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
#include "SunwayMRContext.h"
#include "Partition.h"
#include "MappedRDD.h"
#include "IteratorSeq.h"
using std::string;
using std::vector;

template <class U, class T> class MappedRDD;
class SunwayMRContext;

template <class T>
class RDD {
public:
	RDD();
	RDD(SunwayMRContext &c);
	virtual ~RDD();
	virtual vector<Partition*> getPartitions()=0;
	virtual vector<string> preferredLocations(Partition &p)=0;
	virtual IteratorSeq<T> iteratorSeq(Partition &p)=0;

	template <class U> MappedRDD<U, T> map(U (*f)(T));
	T reduce(T (*g)(T, T));

	SunwayMRContext &context;
	vector<Partition*> partitions;
};


#endif /* RDD_H_ */
