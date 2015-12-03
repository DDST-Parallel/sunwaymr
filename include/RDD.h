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

template <class T>
class RDD {
public:
	RDD(SunwayMRContext &c);
	virtual ~RDD();
	virtual vector<Partition> getPartitions();
	virtual vector<string> preferredLocations(Partition &p const);
	virtual IteratorSeq<T> iteratorSeq(Partition &p const);

	template <class U> MappedRDD<U, T> map(U (*f(T)));
	T reduce(T (*g(T, T)));

	SunwayMRContext &context;
	vector<Partition> partitions;
};


#endif /* RDD_H_ */
