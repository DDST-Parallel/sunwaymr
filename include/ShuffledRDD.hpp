/*
 * ShuffledRDD.hpp
 *
 *  Created on: 2016年2月24日
 *      Author: knshen
 */

#ifndef INCLUDE_SHUFFLEDRDD_HPP_
#define INCLUDE_SHUFFLEDRDD_HPP_

#include "ShuffledRDD.h"

#include <ctime>
#include <cstdlib>
#include <map>
#include <new>

#include "IteratorSeq.hpp"
#include "VectorIteratorSeq.hpp"
#include "Partition.hpp"
#include "ShuffledPartition.hpp"
#include "RDD.hpp"
#include "Pair.hpp"
#include "SunwayMRContext.hpp"
#include "ShuffledTask.hpp"
#include "Aggregator.hpp"
#include "HashDivider.hpp"
#include "TaskResult.hpp"
#include "Task.hpp"
#include "Messaging.hpp"
#include "MessageType.hpp"
#include "Utils.hpp"
#include "TaskScheduler.hpp"
#include "VectorAutoPointer.hpp"

using namespace std;

/*
 * constructor
 */
template <class K, class V, class C>
ShuffledRDD<K, V, C>::ShuffledRDD(RDD< Pair<K, V> > *_prevRDD,
		Aggregator< Pair<K, V>, Pair<K, C> > &_agg,
		HashDivider &_hd,
		long (*hf)(Pair<K, C> &p),
		string (*strf)(Pair<K, C> &p),
		Pair<K, C> (*_recoverFunc)(string &s))
: RDD< Pair<K, C> >::RDD(_prevRDD->context), prevRDD(_prevRDD), agg(_agg), hd(_hd)
{
	hashFunc = hf;
	strFunc = strf;
	shuffleID = this->rddID;
	shuffleFinished = false;
	recoverFunc = _recoverFunc;

	// generate new partitions and initialize mutex
	vector<Partition*> parts;
	for(int i = 0; i < hd.getNumPartitions(); i++)
	{
		Partition *part = new ShuffledPartition(this->rddID, i);
		parts.push_back(part);

		this->shuffleMutexes.push_back(pthread_mutex_t());
		pthread_mutex_init(&this->shuffleMutexes.back(), NULL);
	}
	this->partitions = parts;

	// construct shuffle tasks
	vector<Partition*> pars = prevRDD->getPartitions(); //partitions before shuffle
	for (unsigned int i = 0; i < pars.size(); i++)
	{
		//ShuffleTask(RDD<T> &r, Partition &p, long shID, int nPs, HashDivider &hashDivider, Aggregator<T, U> &aggregator, long (*hFunc)(U), string (*sf)(U));
		ShuffledTask< Pair<K, V>, Pair<K, C> > *task =
				new ShuffledTask< Pair<K, V>, Pair<K, C> >(
						prevRDD, pars[i], this->shuffleID, hd.getNumPartitions(),
						hd, agg, hashFunc, strFunc);
		shuffledTasks.push_back(task);
	}
}

/*
 * destructor.
 * deleting all the shuffle tasks and iteratorSeq cache.
 * deleting the previous RDD if that is not sticky.
 */
template <class K, class V, class C>
ShuffledRDD<K, V, C>::~ShuffledRDD()
{
	for(size_t i = 0; i < this->shuffledTasks.size(); i++) {
		delete this->shuffledTasks[i];
	}
	this->shuffledTasks.clear();

	typename map<int, IteratorSeq< Pair<K, C> >* >::iterator it2;
	for (it2=this->shuffleCache.begin(); it2!=this->shuffleCache.end(); ++it2) {
		delete (it2->second);
	}
	this->shuffleCache.clear();

	if(this->prevRDD != NULL) {
		delete this->prevRDD;
	}
}

/*
 * to get partitions of this RDD.
 * as to ShuffledRDD, the partitions stored in itself, no its previous RDD.
 */
template <class K, class V, class C>
vector<Partition*> ShuffledRDD<K, V, C>::getPartitions()
{
	return this->partitions;
}

/*
 * to get the preferred locations of a partition
 */
template <class K, class V, class C>
vector<string> ShuffledRDD<K, V, C>::preferredLocations(Partition *p)
{
	vector<string> ve;
	return ve;
}

/*
 * shuffle the data set of previous RDD.
 * to create and run ShuffledTasks on previous RDD's partitions.
 */
template <class K, class V, class C>
void ShuffledRDD<K, V, C>::shuffle()
{
	XYZ_TASK_SCHEDULER_RUN_TASK_MODE = 1;

	if (shuffleFinished) return; // shuffle was done before

	prevRDD->shuffle(); // firstly, the previous RDD must do the shuffle

	// run tasks via context
	vector< Task<int> *> tasks;
	for(unsigned int i = 0; i < shuffledTasks.size(); i++) {
		tasks.push_back(shuffledTasks[i]);
	}
	vector< TaskResult<int>* > results = this->context->runTasks(tasks);
	VectorAutoPointer< TaskResult<int> > auto_ptr2(results); // delete pointers automatically

    // cache tasks
	for(unsigned int i = 0; i < shuffledTasks.size(); i++) {
		this->context->saveShuffleCache(this->shuffleID, shuffledTasks[i]);
	}

	this->shuffleFinished = true;

	// !!! as long as shuffle is done, the previous RDD can be destroyed
	if(!this->prevRDD->isSticky()) {
		delete this->prevRDD;
		this->prevRDD = NULL;
	}
}

/*
 * to get data set of a partition.
 * this is done by several steps:
 *   1) to fetch combiners from other nodes
 *   2) to merge the combiners with the same key by Aggregator::mergeCombiner
 *   3) to same cache and return IteratorSeq of pairs after combination
 *
 * note: cannot save shuffle cache data in memory if using fork !
 */
template <class K, class V, class C>
IteratorSeq< Pair<K, C> > * ShuffledRDD<K, V, C>::iteratorSeq(Partition *p)
{
	ShuffledPartition *srp = dynamic_cast<ShuffledPartition * >(p);

	pthread_mutex_lock(&this->shuffleMutexes[srp->partitionID]);
	// checking cache
	if (shuffleCache.find(srp->partitionID) != shuffleCache.end()) {
		pthread_mutex_unlock(&this->shuffleMutexes[srp->partitionID]);
		return this->shuffleCache[srp->partitionID];
	}

	unordered_map<K, C> combiners;
	typename unordered_map<K, C>::iterator iter;

	// merge local data
	for(size_t i = 0; i < this->shuffledTasks.size(); i++) {
		ShuffledTask< Pair<K, V>, Pair<K, C> > * task =
				this->shuffledTasks[i];
		IteratorSeq< Pair <K, C > > *data =
				task->getPartitionData(srp->partitionID);
		if(data != NULL) {
			size_t n = data->size();
			for(size_t j = 0; j < n; j++) {
				Pair<K, C> p = data->at(j);

				iter = combiners.find(p.v1);
				if(iter != combiners.end())
				{
					// the key exists
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

	// fetch
	vector<string> replys;
	vector<string> IPs = (this->context)->getHosts();
	int port = (this->context)->getListenPort();

	string self = getLocalHost();
	string str_shuffleID = num2string(shuffleID);
	string str_partitionID = num2string(srp->partitionID);
	string sendMsg = str_shuffleID + "," + str_partitionID; //organize request
	for(unsigned int i=0; i<IPs.size(); i++)
	{
		if(IPs[i] == self) continue;
		replys.push_back("");
		sendMessageForReply(IPs[i], port, FETCH_REQUEST, sendMsg, replys.back());
	}

	// merge fetched data
	merge(replys, combiners);
	replys.clear();

	// making result 
	vector< Pair<K, C> > ret;
	VectorIteratorSeq< Pair<K, C> > *retIt = new VectorIteratorSeq< Pair<K, C> >(ret);
	typename unordered_map<K, C>::iterator it;
	for(it=combiners.begin(); it!=combiners.end(); it++)
	{
		K k = it->first;
		C c = it->second;
		Pair<K, C> p(k, c);
		retIt->push_back(p);
	}
	combiners.clear();

	// saving cache
	this->shuffleCache[srp->partitionID] = retIt;
	pthread_mutex_unlock(&this->shuffleMutexes[srp->partitionID]);

	return retIt;
}

/*
 * definition of hash structs that may be used by unordered_map
 */
namespace std {
	namespace tr1 {
		template <class K, class V>
		struct hash< Pair<K, V> > : public std::unary_function<Pair<K, V>, size_t>
	    {
	      size_t operator()(const Pair<K, V> &p) const {
	    	  return std::tr1::hash<K>()(p.v1) ^ (std::tr1::hash<V>()(p.v2) << 1);
	      }
	    };

		template <class T>
		struct hash< IteratorSeq<T> > : public std::unary_function<IteratorSeq<T>, size_t>
		{
		  size_t operator()(const IteratorSeq<T> &s) const {
			  size_t ret = std::tr1::hash<size_t>()(s.size());
			  if(s.size() > 0) {
				  for(size_t i = 0; i < s.size(); i++) {
					  ret ^ std::tr1::hash<T>()(s.at(i));
				  }
			  }
			  return ret;
		  }
		};
	}
}

/*
 * to merge combiners fetched from other nodes
 */
template <class K, class V, class C>
void ShuffledRDD<K, V, C>::merge(vector<string> &replys, unordered_map<K, C> &combiners)
{
	int invalid = 0;
	typename unordered_map<K, C>::iterator iter;
	for(unsigned int i=0; i<replys.size(); i++)
	{
		vector<string> pairs;
		splitString(replys[i], pairs, SHUFFLETASK_KV_DELIMITATION);
		for(unsigned int j=0; j<pairs.size(); j++)
		{
			if(pairs[j] == string(SHUFFLETASK_EMPTY_DELIMITATION))
				continue;

			Pair<K, C> p;
			try {
				p = recoverFunc(pairs[j]);
			} catch (std::bad_alloc& ba) {
				invalid ++;
				continue; // converting from string failed
			}
			if (!p.valid) {
				invalid ++;
				continue; // converting from string failed
			}

			iter = combiners.find(p.v1);
			if(iter != combiners.end())
			{
				// the key exists
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
	if (invalid > 0) {
        stringstream ss;
        ss << invalid << " invalid pairs found in ShuffledRDD::merge()";
        Logging::logWarning(ss.str());
	}
}

/*
 * for sub-class of Messaging, must override messageReceived
 */
template <class K, class V, class C>
void ShuffledRDD<K, V, C>::messageReceived(int localListenPort, string fromHost, int msgType, string &msg)
{
}

#endif /* INCLUDE_SHUFFLEDRDD_HPP_ */
