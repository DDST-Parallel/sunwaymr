/*
 * RDD.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef RDD_H_
#define RDD_H_

#include <string>
#include <vector>

#include "IteratorSeq.h"
#include "MappedRDD.h"
#include "FlatMappedRDD.h"
#include "Partition.h"
#include "SunwayMRContext.h"
using std::string;
using std::vector;


template <class U, class T> class MappedRDD;
template <class U, class T> class FlatMappedRDD;
class SunwayMRContext;

template <class T>
class RDD {
public:
	RDD(SunwayMRContext &c);
	virtual ~RDD();
	virtual vector<Partition*> getPartitions()=0;
	virtual vector<string> preferredLocations(Partition &p)=0;
	virtual IteratorSeq<T> iteratorSeq(Partition &p)=0;

	template <class U> MappedRDD<U, T> map(U (*f)(T));
	template <class U> FlatMappedRDD<U, T> flatMap(vector<U> (*f)(T));
	T reduce(T (*g)(T, T));

	SunwayMRContext &context;
	vector<Partition*> partitions;
};


#endif /* RDD_H_ */
