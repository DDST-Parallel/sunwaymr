/*
 * UnionPartition.h
 *
 *  Created on: Feb 26, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_UNIONPARTITION_H_
#define HEADERS_UNIONPARTITION_H_

#include <vector>
#include <string>

#include "IteratorSeq.h"
#include "Partition.h"
using std::vector;
using std::string;

/*
 * Partition of UnionRDD.
 * Constructed from a pointer *RDD and a pointer *partition.
 * The RDD and partition above is from UnionRDD.
 */
template <class T>
class UnionPartition: public Partition {
public:
	UnionPartition(long rddID, int partitionID, RDD<T> *rdd, Partition *partition);
	IteratorSeq<T> * iteratorSeq();
	vector<string> preferredLocations();

	long rddID;
	int partitionID;
	RDD<T> *rdd;
	Partition *partition;
};



#endif /* HEADERS_UNIONPARTITION_H_ */
