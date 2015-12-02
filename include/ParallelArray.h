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
using std::vector;

template <class T>
class ParallelArray : public RDD<T> {
public:
	ParallelArray(SunwayMRContext c, T t1, T t2, int numSlices);
	vector<Partition> getPartitions();
	vector<string> preferredLocations(Partition p);
	vector<T> iteratorArray(Partition p);

};


#endif /* PARALLELARRAY_H_ */
