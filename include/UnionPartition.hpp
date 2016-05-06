/*
 * UnionRDDPartition.hpp
 *
 *  Created on: Feb 26, 2016
 *      Author: yupeng
 */

#ifndef INCLUDE_UNIONPARTITION_HPP_
#define INCLUDE_UNIONPARTITION_HPP_

#include "UnionPartition.h"

#include <iostream>

#include "IteratorSeq.hpp"
#include "Partition.hpp"
using namespace std;

/*
 * constructor
 */
template <class T>
UnionPartition<T>::UnionPartition(long rddID, int partitionID, RDD<T> *rdd, Partition *partition)
:rddID(rddID), partitionID(partitionID), rdd(rdd), partition(partition) {

}

/*
 * to get data of this partition
 */
template <class T>
IteratorSeq<T> * UnionPartition<T>::iteratorSeq() {
	return rdd->iteratorSeq(partition);
}

/*
 * to get preferred locations of this partition
 */
template <class T>
vector<string> UnionPartition<T>::preferredLocations() {
	return rdd->preferredLocations(partition);
}

#endif /* INCLUDE_UNIONPARTITION_HPP_ */
