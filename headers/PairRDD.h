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
#include "VectorIteratorSeq.h"
#include "Partition.h"
#include "RDD.h"
#include "Pair.h"
#include "MappedRDD.h"
#include "Either.h"

using std::vector;
using std::string;

template <class T> class RDD;
template <class U, class T> class MappedRDD;

/*
 * Return type of RDD::mapToPair.
 * PairRDD values are all Pairs.
 */
template <class K, class V, class T>
class PairRDD : public RDD< Pair<K, V> > {
public:
	PairRDD(RDD<T> *prev, Pair<K, V> (*f)(T&));
	~PairRDD();
	vector<Partition *> getPartitions();
	vector<string> preferredLocations(Partition *p);
	IteratorSeq< Pair<K, V> > * iteratorSeq(Partition *p);
	void shuffle();

	template <class U>
	PairRDD<K, U, Pair<K, V> > * mapValues(Pair<K, U> (*f)(Pair<K, V> &)); // change value's type

	MappedRDD<V, Pair< K, V > > * values(); // get all values

	template <class C>
	PairRDD<K, C, Pair<K, C> > * combineByKey(
			Pair<K, C> (*createCombiner)(Pair<K, V>&),
			Pair<K, C> (*mergeCombiner)(Pair<K, C>&, Pair<K, C>&),
			int numPartitions); // used by redueceByKey and groupByKey

	PairRDD<K, V, Pair<K, V> > * reduceByKey(
			Pair<K, V> (*reduce_function)(Pair<K, V>&, Pair<K, V>&),
			int numPartitions); // shuffle operator

	PairRDD<K, V, Pair<K, V> > * reduceByKey(Pair<K, V> (*reduce_function)(Pair<K, V>&, Pair<K, V>&)); // shuffle operator

	PairRDD<K, VectorIteratorSeq<V>, Pair<K, VectorIteratorSeq<V> > > * groupByKey(int num_partitions); // shuffle operator

	PairRDD<K, VectorIteratorSeq<V>, Pair<K, VectorIteratorSeq<V> > > * groupByKey(); // shuffle operator

	template <class W>
	PairRDD< K, Pair< V, W >, Pair< K, Pair< V, W > > > * join(
			RDD< Pair< K, W > > *other,
			int num_partitions); // join two PairRDD

	template <class W>
	PairRDD< K, Pair< V, W >, Pair< K, Pair< V, W > > > * join(
			RDD< Pair< K, W > > *other); // join two PairRDD

private:
	RDD<T> *prevRDD;
	Pair<K, V> (*mapToPairFunction)(T&);
};



#endif /* HEADERS_PAIRRDD_H_ */
