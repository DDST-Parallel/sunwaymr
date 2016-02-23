/*
 * PairRDD.h
 *
 *  Created on: Jan 13, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_PAIRRDD_H_
#define HEADERS_PAIRRDD_H_

#include <vector>
#include <string>

#include "IteratorSeq.h"
#include "Partition.h"
#include "RDD.h"
#include "Pair.h"
using std::vector;
using std::string;

template <class T> class RDD;

template <class K, class V, class T>
class PairRDD : public RDD< Pair<K, V> > {
public:
	PairRDD(RDD<T> &prev, Pair<K, V> (*f)(T));
	vector<Partition*> getPartitions();
	vector<string> preferredLocations(Partition &p);
	IteratorSeq< Pair<K, V> > iteratorSeq(Partition &p);
	void shuffle();

	// TODO join reduceByKey groupByKey mapValues
	template <class U>
	PairRDD<K, U, T> mapValues(Pair<K, U> (*f)(Pair<K, V>));

	PairRDD<K, V, T> reduceByKey(V (*f)(V, V), int num_partitions);

	// PairRDD<K, IteratorSeq<V>, T> groupByKey(int num_partitions);

private:
	RDD<T> &prevRDD;
	Pair<K, V> (*mapToPairFunction)(T);
};



#endif /* HEADERS_PAIRRDD_H_ */
