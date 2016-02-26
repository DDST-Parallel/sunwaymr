/*
 * PairRDD.hpp
 *
 *  Created on: Jan 13, 2016
 *      Author: yupeng
 */

#ifndef PAIRRDD_HPP_
#define PAIRRDD_HPP_

#include "PairRDD.h"

#include <iostream>
#include <vector>
#include "IteratorSeq.hpp"
#include "Partition.hpp"
#include "RDD.hpp"
#include "Pair.hpp"
#include "ShuffledRDD.hpp"
#include "Aggregator.hpp"
#include "HashDivider.hpp"
#include "Either.hpp"
#include "MappedRDD.hpp"

using namespace std;

template <class K, class V, class T>
PairRDD<K, V, T>::PairRDD(RDD<T> &prev, Pair<K, V> (*f)(T))
:RDD< Pair<K, V> >::RDD(prev.context), prevRDD(prev)
{
	mapToPairFunction = f;
}

template <class K, class V, class T>
void PairRDD<K, V, T>::shuffle()
{
	prevRDD.shuffle();
}

template <class K, class V, class T>
vector<Partition*> PairRDD<K, V, T>::getPartitions()
{
	return prevRDD.getPartitions();
}

template <class K, class V, class T>
vector<string> PairRDD<K, V, T>::preferredLocations(Partition &p)
{
	return prevRDD.preferredLocations(p);
}

template <class K, class V, class T>
IteratorSeq< Pair<K, V> >  PairRDD<K, V, T>::iteratorSeq(Partition &p)
{
	IteratorSeq<T> seq1 = prevRDD.iteratorSeq(p);
	return seq1.map(mapToPairFunction);
}

template <class K, class V, class T>
template <class U>
PairRDD<K, U, T> PairRDD<K, V, T>::mapValues(Pair<K, U> (*f)(Pair<K, V>))
{
	PairRDD<K, U, T> pair_rdd(*this, f);
	return pair_rdd;
}

template <class K, class V>
V values_inner_map_f (Pair< K, V > p) {
	return p.v2;
}

template <class K, class V, class T>
MappedRDD<V, Pair< K, V > > PairRDD<K, V, T>::values() {
	return this->map(values_inner_map_f< K, V >);
}

template <class K, class C>
long combineByKey_inner_hash_f (Pair<K, C> p) {
	stringstream ss;
	ss << p.v1;
	return hash(ss.str());
}

template <class K, class C>
string combineByKey_inner_to_string_f (Pair<K, C> p) {
	stringstream ss;
	ss << p;
	return ss.str();
}

template <class K, class C>
Pair<K, C> combineByKey_inner_from_string_f (string s) {
	Pair<K, C> p;
	stringstream ss(s);
	ss >> p;
	return p;
}

template <class K, class V, class T>
template <class C>
ShuffledRDD<K, V, C> PairRDD<K, V, T>::combineByKey(
		Pair<K, C> (*createCombiner)(Pair<K, V>),
		Pair<K, C> (*mergeCombiner)(Pair<K, C>, Pair<K, C>),
		int numPartitions)
 {
	Aggregator< Pair<K, V>, Pair<K, C> > *agg = new Aggregator< Pair<K, V>, Pair<K, C> >(createCombiner, mergeCombiner);
	HashDivider *hd = new HashDivider(numPartitions);
	ShuffledRDD<K, V, C> shuffledRDD(*this, *agg, *hd, combineByKey_inner_hash_f<K, C>, combineByKey_inner_to_string_f<K, C>, this->rddID, combineByKey_inner_from_string_f<K, C>);
	return shuffledRDD;
 }

template <class K, class V>
Pair<K, V> do_nothing(Pair<K, V> p)
{
	return p;
}

template <class K, class V, class T>
ShuffledRDD<K, V, V> PairRDD<K, V, T>::reduceByKey(
		Pair<K, V> (*reduce_function)(Pair<K, V>, Pair<K, V>),
		int numPartitions)
{
	return combineByKey(do_nothing<K, V>, reduce_function, numPartitions);
}

template <class K, class V, class T>
ShuffledRDD<K, V, V> PairRDD<K, V, T>::reduceByKey(
		Pair<K, V> (*reduce_function)(Pair<K, V>, Pair<K, V>))
{
	return combineByKey(do_nothing<K, V>, reduce_function, (this->context).getTotalThreads());
}

template <class K, class V>
Pair<K, IteratorSeq<V> > groupByKey_inner_createCombiner ( Pair<K, V> p) {
	vector<V> vv;
	vv.push_back(p.v2);
	IteratorSeq<V> iv(vv);
	return Pair<K, IteratorSeq<V> >(p.v1, iv);
}

template <class K, class V>
Pair<K, IteratorSeq<V> > groupByKey_inner_mergeCombiner (
		Pair<K, IteratorSeq<V> > p1,
		Pair<K, IteratorSeq<V> > p2) {
	vector<V> vv1 = p1.v2.getVector();
	vector<V> vv2 = p2.v2.getVector();
	vv1.insert(vv1.end(), vv2.begin(), vv2.end());
	IteratorSeq<V> iv(vv1);
	return Pair<K, IteratorSeq<V> >(p1.v1, iv);
}

template <class K, class V, class T>
ShuffledRDD<K, V, IteratorSeq<V> > PairRDD<K, V, T>::groupByKey(
		int num_partitions) {
	return combineByKey(groupByKey_inner_createCombiner<K, V>,
			groupByKey_inner_mergeCombiner<K, V>,
			num_partitions);

}

template <class K, class V, class T>
ShuffledRDD<K, V, IteratorSeq<V> > PairRDD<K, V, T>::groupByKey() {
	return groupByKey((this->context).getTotalThreads());

}


#endif /* PIARRDD_HPP_ */


