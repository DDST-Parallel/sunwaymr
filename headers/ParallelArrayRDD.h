/*
 * ParallelArrayRDD.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef PARALLELARRAYRDD_H_
#define PARALLELARRAYRDD_H_

#include <vector>
#include <string>

#include "IteratorSeq.h"
#include "ParallelArrayPartition.h"
#include "RDD.h"
#include "SunwayMRContext.h"
using std::vector;
using std::string;

template <class T> class RDD;
template <class U, class T> class MappedRDD;
class SunwayMRContext;

/*
 * A RDD type with IteratorSeq values, e.g. 1...1000
 */
template <class T>
class ParallelArrayRDD : public RDD<T> {
public:
	ParallelArrayRDD(SunwayMRContext *c, IteratorSeq<T> *seq, int numSlices);
	vector<Partition *> getPartitions();
	vector<string> preferredLocations(Partition *p);
	IteratorSeq<T> * iteratorSeq(Partition *p);

	long parallelArray_id;

private:
	//data
	int numSlices;
	vector< IteratorSeq<T>* > slice(IteratorSeq<T> *seq);
};


#endif /* PARALLELARRAYRDD_H_ */
