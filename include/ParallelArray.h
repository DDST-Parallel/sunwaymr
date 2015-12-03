/*
 * ParallelArray.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef PARALLELARRAY_H_
#define PARALLELARRAY_H_

#include <vector>
#include "SunwayMRContext.h"
#include "IteratorSeq.h"
using std::vector;

template <class T>
class ParallelArray : public RDD<T> {
public:
	ParallelArray(SunwayMRContext &c, IteratorSeq<T> &seq, int numSlices);
	vector<Partition> getPartitions();
	vector<string> preferredLocations(Partition &p const);
	IteratorSeq<T> iteratorSeq(Partition &p const);

};


#endif /* PARALLELARRAY_H_ */
