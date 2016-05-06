/*
 * UnionRDD.h
 *
 *  Created on: Feb 26, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_UNIONRDD_H_
#define HEADERS_UNIONRDD_H_

#include <vector>
#include <string>

#include "IteratorSeq.h"
#include "Partition.h"
#include "SunwayMRContext.h"
#include "RDD.h"
using std::vector;
using std::string;

template <class T> class RDD;
class SunwayMRContext;

/*
 * Return type of RDD::unionRDD.
 * To union numbers of RDDs.
 * Create UnionPartitions for each RDD and each partition in RDDs.
 */
template <class T>
class UnionRDD : public RDD<T> {
public:
	UnionRDD(SunwayMRContext *context, vector< RDD<T>* > &rdds);
	~UnionRDD();
	vector<Partition*> getPartitions();
	vector<string> preferredLocations(Partition *p);
	IteratorSeq<T> * iteratorSeq(Partition *p);
	void shuffle();

private:
	vector< RDD<T>* > rdds;
};



#endif /* HEADERS_UNIONRDD_H_ */
