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
#include "ShuffledRDD.h"
#include "MappedRDD.h"

using std::vector;
using std::string;

template <class K, class V, class C>
Pair<K, C> do_nothing(Pair<K, V> p);


template <class T> class RDD;

template <class K, class V, class T>
class PairRDD : public RDD< Pair<K, V> > {
public:
	PairRDD(RDD<T> &prev, Pair<K, V> (*f)(T));
	vector<Partition*> getPartitions();
	vector<string> preferredLocations(Partition &p);
	IteratorSeq< Pair<K, V> > iteratorSeq(Partition &p);
	void shuffle();

	template <class U>
	PairRDD<K, U, T> mapValues(Pair<K, U> (*f)(Pair<K, V>));

	MappedRDD<V, Pair< K, V > > values();

	template <class C>
	ShuffledRDD<K, V, C> combineByKey(Pair<K, C> (*createCombiner)(Pair<K, V>),
			Pair<K, C> (*mergeCombiner)(Pair<K, C>, Pair<K, C>),
			int numPartitions);

	ShuffledRDD<K, V, V> reduceByKey(
			Pair<K, V> (*reduce_function)(Pair<K, V>, Pair<K, V>),
			int numPartitions);

	ShuffledRDD<K, V, V> reduceByKey(Pair<K, V> (*reduce_function)(Pair<K, V>, Pair<K, V>));

	ShuffledRDD<K, V, IteratorSeq<V> > groupByKey(int num_partitions);

	ShuffledRDD<K, V, IteratorSeq<V> > groupByKey();

private:
	RDD<T> &prevRDD;
	Pair<K, V> (*mapToPairFunction)(T);
};



#endif /* HEADERS_PAIRRDD_H_ */
