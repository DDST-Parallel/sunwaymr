/*
 * FlatMappedRDD.h
 *
 * RDD::flatmap will return a FlatMappedRDD
 *
 *  Created on: Jan 13, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_FLATMAPPEDRDD_H_
#define HEADERS_FLATMAPPEDRDD_H_

#include <vector>
#include <string>

#include "IteratorSeq.h"
#include "Partition.h"
#include "RDD.h"
using std::vector;
using std::string;

template <class T> class RDD;

template <class U, class T>
class FlatMappedRDD : public RDD<U> {
public:
	FlatMappedRDD(RDD<T> *prev, vector<U> (*f)(T&));
	~FlatMappedRDD();
	vector<Partition *> getPartitions();
	vector<string> preferredLocations(Partition *p);
	IteratorSeq<U> * iteratorSeq(Partition *p);
	void shuffle();

private:
	RDD<T> *prevRDD;
	vector<U> (*mappedFunction)(T&);
};


#endif /* HEADERS_FLATMAPPEDRDD_H_ */
