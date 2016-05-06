/*
 * AllNodesPartition.hpp
 *
 *  Created on: Jan 17, 2016
 *      Author: yupeng
 */

#ifndef ALLNODESPARTITION_HPP_
#define ALLNODESPARTITION_HPP_

#include "AllNodesPartition.h"

#include <iostream>
#include "IteratorSeq.hpp"
#include "Partition.hpp"
#include "PointerContainer.hpp"
using namespace std;

/*
 * constructor
 */
template <class T>
AllNodesPartition<T>::AllNodesPartition(long _rddID, int _partitionID, IteratorSeq<PointerContainer <T> > *_values)
: rddID(_rddID), partitionID(_partitionID), values(_values)
{

}

/*
 * destructor
 */
template <class T>
AllNodesPartition<T>::~AllNodesPartition() {
	// values will be deleted in AllNodesRDD's destructor.
	// so, do nothing here.
}

/*
 * to obtain data stored in this partition
 */
template <class T>
IteratorSeq<PointerContainer <T> > * AllNodesPartition<T>::iteratorSeq()
{
	return values;
}

#endif /* ALLNODESPARTITION_HPP_ */

