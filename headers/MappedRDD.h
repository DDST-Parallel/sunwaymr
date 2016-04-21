/*
 * MappedRDD.h
 *
 * Return type of RDD::map.
 * Mapping mappedFunction to each value of RDD.
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef MAPPEDRDD_H_
#define MAPPEDRDD_H_

#include <vector>
#include <string>

#include "IteratorSeq.h"
#include "Partition.h"
#include "RDD.h"
using std::vector;
using std::string;

template <class T> class RDD;

template <class U, class T>
class MappedRDD : public RDD<U> {
public:
	MappedRDD(RDD<T> *prev, U (*f)(T&));
	~MappedRDD();
	vector<Partition*> getPartitions();
	vector<string> preferredLocations(Partition *p);
	IteratorSeq<U> * iteratorSeq(Partition *p);
	void shuffle();

private:
	RDD<T> *prevRDD;
	U (*mappedFunction)(T&);
};



#endif /* MAPPEDRDD_H_ */
