#ifndef PARALLELARRAYPARTITION_HPP_
#define PARALLELARRAYPARTITION_HPP_


#include "ParallelArrayPartition.h"

#include <iostream>

#include "IteratorSeq.hpp"
#include "Partition.hpp"
using namespace std;

/*
 * constructor
 */
template <class T>
ParallelArrayPartition<T>::ParallelArrayPartition(long _rddID, int _partitionID, IteratorSeq<T> *_values)
: rddID(_rddID), partitionID(_partitionID), values(_values)
{
}

/*
 * destructor
 */
template <class T>
ParallelArrayPartition<T>::~ParallelArrayPartition() {
	delete this->values;
}

/*
 * to get data in this partition
 */
template <class T>
IteratorSeq<T> * ParallelArrayPartition<T>::iteratorSeq()
{
	return this->values;
}



#endif /* PARALLELARRAYPARTITION_HPP_ */
