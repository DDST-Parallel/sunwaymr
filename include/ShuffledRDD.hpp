/*
 * ShuffledRDD.hpp
 *
 *  Created on: 2016年2月24日
 *      Author: knshen
 */

#ifndef INCLUDE_SHUFFLEDRDD_HPP_
#define INCLUDE_SHUFFLEDRDD_HPP_

#include "ShuffledRDD.h"
#include "IteratorSeq.hpp"
#include "Partition.hpp"
#include "RDD.hpp"
#include "Pair.hpp"
#include "SunwayMRContext.hpp"
#include "ShuffledRDDPartition.hpp"
#include "Aggregator.hpp"
#include "HashDivider.hpp"
#include "ShuffleTask.hpp"
#include "TaskResult.hpp"
#include "Task.hpp"
#include "Messaging.hpp"
#include "MessageType.hpp"
#include "Utils.hpp"

#include <ctime>
#include <cstdlib>
#include <map>
#include <strstream>
using namespace std;

template <class K, class V, class C>
ShuffledRDD<K, V, C>::ShuffledRDD(RDD< Pair<K, V> > &_preRDD, Aggregator< Pair<K, V>, Pair<K, C> > &_agg, HashDivider &_hd, long (*hf)(Pair<K, C>), string (*strf)(Pair<K, C>), long _shuffleID, Pair<K, C> (*_recoverFunc)(string))
: RDD< Pair<K, C> >::RDD(_preRDD.context)
{
	preRDD = _preRDD;
	agg = _agg;
	hd = _hd;
	hashFunc = hf;
	strFunc = strf;
	shuffleID = _shuffleID;
	recoverFunc = _recoverFunc;

	// generate new partitions
	vector<Partition*> parts;
	for(int i=0; i<hd.numPartitions; i++)
	{
		Partition *part = new ShuffledRDDPartition(this->rddID, i);
		parts.push_back(part);
	}
	this->partitions = parts;

	srand(unsigned(time(0)));
}

template <class K, class V, class C>
vector<Partition*> ShuffledRDD<K, V, C>::getPartitions()
{
	return this->partitions;
}

template <class K, class V, class C>
vector<string> ShuffledRDD<K, V, C>::preferredLocations(Partition &p)
{
	vector<string> ve;
	return ve;
}

template <class K, class V, class C>
void ShuffledRDD<K, V, C>::shuffle()
{
	preRDD.shuffle();

	// construct tasks
	vector< Task<int>* > tasks;
	vector<Partition*> pars = preRDD.partitions; //partitions before shuffle

	for (int i = 0; i < pars.size(); i++)
	{
		//ShuffleTask(RDD<T> &r, Partition &p, long shID, int nPs, HashDivider &hashDivider, Aggregator<T, U> &aggregator, long (*hFunc)(U), string (*sf)(U));
		Task<int> *task = new ShuffleTask< Pair<K, V>, Pair<K, C> >(*this, *(pars[i]), this->rddID, hd.numPartitions, hd, agg, hashFunc, strFunc);
		tasks.push_back(task);
	}

	// run tasks via context
	(this->context).runTasks(tasks);
}

template <class K, class V, class C>
IteratorSeq< Pair<K, C> > ShuffledRDD<K, V, C>::iteratorSeq(Partition &p)
{
	ShuffledRDDPartition &srp = dynamic_cast<ShuffledRDDPartition&>(p);

	// fetch
	vector<string> IPs = (this->context).getHosts();
	int port = (int)getRandom(1025, 65530 - IPs.size()); // get a target port randomly

	string str_shuffleID;
	string str_partitionID;

	strstream ss1;
	strstream ss2;
	ss1<<shuffleID;
	ss1>>str_shuffleID;
	ss2<<srp.partitionID;
	ss2>>str_partitionID;
	string sendMsg = str_shuffleID+","+str_partitionID;

	vector<string> replys;
	for(int i=0; i<IPs.size(); i++)
	{
		string reply;
		sendMessageForReply(IPs[i], port++, FETCH_REQUEST, sendMsg, reply);
		replys.push_back(reply);
	}

	// merge
	vector< Pair<K, C> > ret;
	map<K, C> combiners = merge(replys);
	typename map<K, C>::iterator it;
	for(it=combiners.begin(); it!=combiners.end(); it++)
	{
		Pair<K, C> p;
		p.v1 = it->first;
		p.v2 = it->second;
		ret.push_back(p);
	}
	IteratorSeq< Pair<K, C> > retIt(ret);
	return retIt;
}

template <class K, class V, class C>
map<K, C> ShuffledRDD<K, V, C>::merge(vector<string> replys)
{
	map<K, C> combiners;
	for(int i=0; i<replys.size(); i++)
	{
		vector<string> pairs;
		splitString(replys[i], pairs, " ");
		for(int j=0; j<pairs.size(); j++)
		{
			typename map<K, C>::iterator iter;
			Pair<K, C> p = recoverFunc(pairs[j]);
			iter = find(p.v1);
			if(iter != combiners.end())
			{
				// the key exist
				Pair<K, C> origin;
				origin.v1 = p.v1;
				origin.v2 = combiners[p.v1];
				Pair<K, C> newPair = agg.mergeCombiners(origin, p);
				combiners[p.v1] = newPair.v2;
			}
			else
			{
				combiners[p.v1] = p.v2;
			}
		}
	}

	return combiners;
}

template <class K, class V, class C>
double ShuffledRDD<K, V, C>::getRandom(double start, double end)
{
	return start+(end-start)*rand()/(RAND_MAX + 1.0);
}

#endif /* INCLUDE_SHUFFLEDRDD_HPP_ */
