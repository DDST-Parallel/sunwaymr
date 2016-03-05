/*
 * RDD.h
 *
 * Abstract super class of all other RDD classes.
 * With some general interfaces of RDD.
 * So, every RDD has operator: map, flatmap, mapToPair,
 * 		reduce , shuffle, distinct, collect, unionRDD
 *
 * Some operator will return a new RDD.
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
#include "PairRDD.h"
#include "Partition.h"
#include "SunwayMRContext.h"
#include "UnionRDD.h"
using std::string;
using std::vector;


template <class U, class T> class MappedRDD;
template <class U, class T> class FlatMappedRDD;
template <class K, class V, class T> class PairRDD;
class SunwayMRContext;

long XYZ_CURRENT_RDD_ID = 1; // id counter

template <class T>
class RDD {
public:
	RDD(SunwayMRContext &c);
	RDD<T> & operator=(const RDD<T> &p);
	virtual ~RDD();
	virtual void initRDDFrom(const RDD<T> &r);
	virtual vector<Partition*> getPartitions()=0;
	virtual vector<string> preferredLocations(Partition &p)=0;
	virtual IteratorSeq<T> iteratorSeq(Partition &p)=0;

	template <class U> MappedRDD<U, T> & map(U (*f)(T));
	template <class U> FlatMappedRDD<U, T> & flatMap(vector<U> (*f)(T));
	template <class K, class V> PairRDD<K, V, T> & mapToPair(Pair<K, V> (*f)(T));
	T reduce(T (*g)(T, T));
	virtual void shuffle();

	MappedRDD<T, Pair< T, int > > & distinct(int newNumSlices);
	MappedRDD<T, Pair< T, int > > & distinct(); // by default, newNumSlices = partitions.size()
	vector<T>& collect();

	UnionRDD<T> & unionRDD(RDD<T> *other);

	SunwayMRContext &context;
	vector<Partition*> partitions;
	long rddID;
};


#endif /* RDD_H_ */
