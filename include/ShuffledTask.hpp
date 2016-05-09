/*
 * ShuffledTask.hpp
 *
 *  Created on: 2016年2月22日
 *      Author: knshen
 */

#ifndef INCLUDE_SHUFFLEDTASK_HPP_
#define INCLUDE_SHUFFLEDTASK_HPP_

#include "ShuffledTask.h"

#include "RDD.hpp"
#include "Partition.hpp"
#include "IteratorSeq.hpp"
#include "RDDTask.hpp"
#include "DataCache.hpp"
#include "Aggregator.hpp"
#include "HashDivider.hpp"
#include "Utils.hpp"
#include "DataCache.hpp"
#include "VectorIteratorSeq.hpp"

#include <vector>
#include <string>
#include <fstream>
using namespace std;

/*
 * constructor
 */
template <class T, class U> ShuffledTask<T, U>::ShuffledTask(
		RDD<T> *r, Partition *p, long shID, int nPs,
		HashDivider &hashDivider,
		Aggregator<T, U> &aggregator,
		long (*hFunc)(U &u),
		string (*sf)(U &u))
:RDDTask< T, int >::RDDTask(r, p), hd(hashDivider), agg(aggregator)
{
	shuffleID = shID;
    numPartitions = nPs;
	hashFunc = hFunc;
	strFunc = sf;

    for(int i = 0; i < numPartitions; i++)
    {
    	partitions.push_back(new VectorIteratorSeq<U>());
    }
}


/*
 * destructor
 */
template <class T, class U> ShuffledTask<T, U>::~ShuffledTask() {
	for(size_t i = 0; i < partitions.size(); i++) {
		delete partitions[i];
	}
	partitions.clear();
}

/*
 * to run the ShuffledTask.
 * this are several things:
 *   1) create combiners for each element in the partition
 *   2) by hash of each element, choose the new partition index of each element
 *
 * return 1
 */
template <class T, class U> int ShuffledTask<T, U>::run()
{
	// get current RDD value
	IteratorSeq<T> *seq = RDDTask< T, int >::rdd->iteratorSeq(RDDTask< T, int >::partition);
    for(size_t i = 0; i < seq->size(); i++) {
    	T t = seq->at(i);
    	U data = agg.createCombiner(t);
		long hashCode = hashFunc(data);
		int part = hd.getPartition(hashCode); // get the new partition index
		partitions[part]->push_back(data);
    }

	return 1;
}

/*
 * return combiners data of requested partition.
 * serializing each element in the partition.
 */
template <class T, class U>
void ShuffledTask<T, U>::getData(long cacheIndex, string &result) {
	result = "";
	if(cacheIndex >= 0
		&& cacheIndex < numPartitions
		&& partitions[cacheIndex]->size() > 0) {
			size_t n = partitions[cacheIndex]->size();
			for(size_t i = 0; i < n - 1; i++) {
				U u = partitions[cacheIndex]->at(i);
				result += strFunc(u);
				result += string(SHUFFLETASK_KV_DELIMITATION);
			}
			U u = partitions[cacheIndex]->at(n - 1);
			result += strFunc(u);
	}
	else {
		result = SHUFFLETASK_EMPTY_DELIMITATION;
	}
}

/*
 * get combiners data of a partition
 */
template <class T, class U>
IteratorSeq<U> * ShuffledTask<T, U>::getPartitionData(int partition) {
	if(partition < 0 || partition >= numPartitions) {
		return NULL;
	}
	else {
		return partitions[partition];
	}
}

/*
 * serializing the result of ShuffledTask
 */
template <class T, class U> string ShuffledTask<T, U>::serialize(int &t)
{
	return to_string(t);
}

/*
 * deserializing a string to task result
 */
template <class T, class U> int ShuffledTask<T, U>::deserialize(string &s)
{
	int val = 0;
	from_string(val, s);
	return val;
}

#endif /* INCLUDE_SHUFFLEDTASK_HPP_ */
