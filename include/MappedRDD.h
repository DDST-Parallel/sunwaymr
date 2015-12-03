/*
 * MappedRDD.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef MAPPEDRDD_H_
#define MAPPEDRDD_H_

#include <vector>
#include "IteratorSeq.h"
using std::vector;

template <class U, class T>
class MappedRDD : public RDD<U> {
public:
	MappedRDD(RDD<U> &prev, U (*f)(T));
	vector<Partition> getPartitions();
	vector<string> preferredLocations(Partition &p const);
	IteratorSeq<U> iteratorSeq(Partition &p const);

private:
	RDD<T> &prevRDD;
	U (*mappedFunction)(T);
};



#endif /* MAPPEDRDD_H_ */
