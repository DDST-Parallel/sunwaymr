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
#include "TaskScheduler.h"

#include <ctime>
#include <cstdlib>
#include <map>
#include <sstream>
#include <new>
using namespace std;

template <class K, class V, class C>
ShuffledRDD<K, V, C>::ShuffledRDD(RDD< Pair<K, V> > &_preRDD, Aggregator< Pair<K, V>, Pair<K, C> > &_agg, HashDivider &_hd, long (*hf)(Pair<K, C>), string (*strf)(Pair<K, C>), Pair<K, C> (*_recoverFunc)(string))
: RDD< Pair<K, C> >::RDD(_preRDD.context), preRDD(_preRDD), agg(_agg), hd(_hd)
{
	hashFunc = hf;
	strFunc = strf;
	shuffleID = this->rddID;
	shuffleFinished = false;
	recoverFunc = _recoverFunc;

	// generate new partitions
	vector<Partition*> parts;
	for(int i=0; i<hd.getNumPartitions(); i++)
	{
		Partition *part = new ShuffledRDDPartition(this->rddID, i);
		parts.push_back(part);
	}
	this->partitions = parts;
}

template <class K, class V, class C>
ShuffledRDD<K, V, C>::~ShuffledRDD()
{
	typename map<int, IteratorSeq< Pair<K, C> >* >::iterator it;
	for (it=this->shuffleCache.begin(); it!=this->shuffleCache.end(); ++it) {
		delete (it->second);
	}
	this->shuffleCache.clear();

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
	XYZ_TASK_SCHEDULER_RUN_TASK_MODE = 1;

	if (shuffleFinished) return;

	preRDD.shuffle();

	// construct tasks
	vector< Task<int>* > tasks;
	vector<Partition*> pars = preRDD.getPartitions(); //partitions before shuffle

	for (unsigned int i = 0; i < pars.size(); i++)
	{
		//ShuffleTask(RDD<T> &r, Partition &p, long shID, int nPs, HashDivider &hashDivider, Aggregator<T, U> &aggregator, long (*hFunc)(U), string (*sf)(U));
		Task<int> *task = new ShuffleTask< Pair<K, V>, Pair<K, C> >(preRDD, *(pars[i]), this->shuffleID, hd.getNumPartitions(), hd, agg, hashFunc, strFunc);
		tasks.push_back(task);
	}

	// run tasks via context
	(this->context).runTasks(tasks);
	this->shuffleFinished = true;
}

template <class K, class V, class C>
IteratorSeq< Pair<K, C> > ShuffledRDD<K, V, C>::iteratorSeq(Partition &p)
{
	ShuffledRDDPartition &srp = dynamic_cast<ShuffledRDDPartition&>(p);

	// !TODO cannot save shuffle cache data in memory if running in fork !
	// now running tasks by pthread if shuffled (by change XYZ_TASK_SCHEDULER_RUN_TASK_MODE in shuffle())
	if (shuffleCache.find(srp.partitionID) != shuffleCache.end()) {
		return *(this->shuffleCache[srp.partitionID]);
	}

	// fetch
	vector<string> IPs = (this->context).getHosts();
	int port = (this->context).getListenPort();

	string str_shuffleID = num2string(shuffleID);
	string str_partitionID = num2string(srp.partitionID);

	string sendMsg = str_shuffleID+","+str_partitionID; //organize request

	vector<string * > replys;
	for(unsigned int i=0; i<IPs.size(); i++)
	{
		string *reply = new string();
		sendMessageForReply(IPs[i], port, FETCH_REQUEST, sendMsg, *reply);
		replys.push_back(reply);
	}

	// merge
	vector< Pair<K, C> > ret;
	map<K, C> combiners;
	merge(replys, combiners);
	// delete replys 
	for (vector<string * >::iterator it=replys.begin(); it!=replys.end(); ++it) {
		delete(*it);
	}
	replys.clear();
	// making result 
	typename map<K, C>::iterator it;
	for(it=combiners.begin(); it!=combiners.end(); it++)
	{
		Pair<K, C> p(it->first, it->second);
		ret.push_back(p);
	}
	IteratorSeq< Pair<K, C> > *retIt = new IteratorSeq< Pair<K, C> >(ret);

	// saving cache
	this->shuffleCache[srp.partitionID] = retIt;

	return *retIt;
}

template <class K, class V, class C>
void ShuffledRDD<K, V, C>::merge(vector<string * > &replys, map<K, C> &combiners)
{
	for(unsigned int i=0; i<replys.size(); i++)
	{
		vector<string> pairs;
		splitString(*replys[i], pairs, SHUFFLETASK_KV_DELIMITATION);
		for(unsigned int j=0; j<pairs.size(); j++)
		{
			if(pairs[j] == string(SHUFFLETASK_EMPTY_DELIMITATION))
				continue;

			typename map<K, C>::iterator iter;
			Pair<K, C> p;
			try {
				p = recoverFunc(pairs[j]);
			} catch (std::bad_alloc& ba) {
				continue; // converting from string failed
			}
			iter = combiners.find(p.v1);
			if(iter != combiners.end())
			{
				// the key exist
				Pair<K, C> origin(p.v1, combiners[p.v1]);
				Pair<K, C> newPair = agg.mergeCombiners(origin, p);
				combiners[p.v1] = newPair.v2;
			}
			else
			{
				combiners[p.v1] = p.v2;
			}
		}
	}
}

template <class K, class V, class C>
void ShuffledRDD<K, V, C>::messageReceived(int localListenPort, string fromHost, int msgType, string msg)
{
}

#endif /* INCLUDE_SHUFFLEDRDD_HPP_ */
