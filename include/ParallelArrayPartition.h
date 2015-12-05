/*
 * ParallelArrayPartition.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef PARALLELARRAYPARTITION_H_
#define PARALLELARRAYPARTITION_H_

#include <vector>
#include "Partition.h"
#include "IteratorSeq.h"
using std::vector;

template <class T>
class ParallelArrayPartition: public Partition {
public:
	ParallelArrayPartition(long rddID, int partitionID, IteratorSeq<T> &values);
	IteratorSeq<T> iteratorSeq();


	int partitionID;
	long rddID;
	IteratorSeq<T> &values;
};


#endif /* PARALLELARRAYPARTITION_H_ */
