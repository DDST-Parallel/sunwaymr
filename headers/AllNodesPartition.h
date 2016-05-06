/*
 * AllNodesPartition.h
 *
 *  Created on: Jan 17, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_ALLNODESPARTITION_H_
#define HEADERS_ALLNODESPARTITION_H_

#include <vector>

#include "IteratorSeq.h"
#include "Partition.h"
#include "PointerContainer.h"
using std::vector;

/*
 *	Partition of AllNodesRDD
 */
template <class T>
class AllNodesPartition : public Partition {
public:
	AllNodesPartition(long rddID, int partitionID, IteratorSeq<PointerContainer <T> > *values);
	~AllNodesPartition();
	IteratorSeq<PointerContainer <T> > * iteratorSeq(); // get data

	long rddID;
	int partitionID;
	IteratorSeq<PointerContainer <T> > *values; // array of PointerContainer.
};



#endif /* HEADERS_ALLNODESPARTITION_H_ */
