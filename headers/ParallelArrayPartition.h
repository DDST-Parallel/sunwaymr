/*
 * ParallelArrayPartition.h
 *
 * RDD partition of ParallelArray (ParallelArray is a RDD type).
 * With a part of IteratorSeq values of ParallelArray.
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef PARALLELARRAYPARTITION_H_
#define PARALLELARRAYPARTITION_H_

#include <vector>

#include "IteratorSeq.h"
#include "Partition.h"
using std::vector;

template <class T>
class ParallelArrayPartition: public Partition {
public:
	ParallelArrayPartition(long rddID, int partitionID, IteratorSeq<T> *values);
	~ParallelArrayPartition();
	IteratorSeq<T> * iteratorSeq();

	long rddID;
	int partitionID;
private:
	IteratorSeq<T> *values;
};


#endif /* PARALLELARRAYPARTITION_H_ */
