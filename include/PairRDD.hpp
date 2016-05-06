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
#include "VectorIteratorSeq.hpp"
#include "Partition.hpp"
#include "RDD.hpp"
#include "Pair.hpp"
#include "ShuffledRDD.hpp"
#include "Aggregator.hpp"
#include "HashDivider.hpp"
#include "Either.hpp"
#include "MappedRDD.hpp"
#include "UnionRDD.hpp"
#include "StringConversion.hpp"

using namespace std;

/*
 * constructor
 */
template <class K, class V, class T>
PairRDD<K, V, T>::PairRDD(RDD<T> *prev, Pair<K, V> (*f)(T&))
:RDD< Pair<K, V> >::RDD(prev->context), prevRDD(prev)
{
	mapToPairFunction = f;
}

/*
 * destructor, deleting the previous RDD if it is not sticky
 */
template <class K, class V, class T>
PairRDD<K, V, T>::~PairRDD()
{
	if(!this->prevRDD->isSticky()) {
		delete this->prevRDD;
	}
}

/*
 * shuffle data of the previous RDD
 */
template <class K, class V, class T>
void PairRDD<K, V, T>::shuffle()
{
	prevRDD->shuffle();
}

/*
 * get partitions of this RDD.
 * as to PairRDD, all partitions are form its previous RDD
 */
template <class K, class V, class T>
vector<Partition*> PairRDD<K, V, T>::getPartitions()
{
	return prevRDD->getPartitions();
}

/*
 * get the preferred locations of a partition
 */
template <class K, class V, class T>
vector<string> PairRDD<K, V, T>::preferredLocations(Partition *p)
{
	return prevRDD->preferredLocations(p);
}

/*
 * to get data set of a partition.
 * return mapped IteratorSeq of the data set from previous RDD
 */
template <class K, class V, class T>
IteratorSeq< Pair<K, V> > * PairRDD<K, V, T>::iteratorSeq(Partition *p)
{
	IteratorSeq< Pair<K, V> > * ret = prevRDD->iteratorSeq(p)->map(mapToPairFunction);
	this->addIteratorSeq(ret); // for garbage collection
	return ret;
}

/*
 * to create a new PairRDD modifying the type and(or) value of this PairRDD
 */
template <class K, class V, class T>
template <class U>
PairRDD<K, U, Pair<K, V> > * PairRDD<K, V, T>::mapValues(Pair<K, U> (*f)(Pair<K, V>&))
{
	return new PairRDD<K, U, Pair<K, V> >(this, f);
}

/*
 * do nothing for a Pair.
 * used when change a RDD<Pair> to a PairRDD
 */
template <class K, class V>
Pair< K, V > xyz_pair_rdd_do_nothing_f(Pair< K, V > &p) {
	return p;
}

/*
 * map function used by values.
 * return right side of a pair.
 */
template <class K, class V>
V xyz_pair_rdd_values_inner_map_f (Pair< K, V > &p) {
	return p.v2;
}

/*
 * mapping all the right side values of this PairRDD.
 * return a newly created MappedRDD.
 */
template <class K, class V, class T>
MappedRDD<V, Pair< K, V > > * PairRDD<K, V, T>::values() {
	return this->map(xyz_pair_rdd_values_inner_map_f< K, V >);
}

/*
 * combine by key hash function.
 * return the hash of left side key of a pair
 */
template <class K, class C>
long xyz_pair_rdd_combine_by_key_inner_hash_f (Pair<K, C> &p) {
	long ret = std::tr1::hash<string>()(to_string(p.v1));
	return ret;
}

/*
 * combine by key to_string function
 */
template <class K, class C>
string xyz_pair_rdd_combine_by_key_inner_to_string_f (Pair<K, C> &p) {
	return to_string(p);
}

/*
 * combine by key from_string function
 */
template <class K, class C>
Pair<K, C> xyz_pair_rdd_combine_by_key_inner_from_string_f (string &s) {
	Pair<K, C> p;
	from_string(p, s);
	return p;
}

/*
 * combineByKey is depended by reduceByKey, groupByKey.
 * combineByKey will create a ShuffledRDD.
 * a ShuffledRDD can shuffle data set of current RDD to new partitions in the new ShuffledRDD.
 */
template <class K, class V, class T>
template <class C>
PairRDD<K, C, Pair<K, C> > * PairRDD<K, V, T>::combineByKey(
		Pair<K, C> (*createCombiner)(Pair<K, V>&),
		Pair<K, C> (*mergeCombiner)(Pair<K, C>&, Pair<K, C>&),
		int numPartitions)
 {
	Aggregator< Pair<K, V>, Pair<K, C> > agg(createCombiner, mergeCombiner);
	HashDivider hd(numPartitions);
	ShuffledRDD<K, V, C> *shuffledRDD =
			new ShuffledRDD<K, V, C>(
					this,
					agg,
					hd,
					xyz_pair_rdd_combine_by_key_inner_hash_f<K, C>,
					xyz_pair_rdd_combine_by_key_inner_to_string_f<K, C>,
					xyz_pair_rdd_combine_by_key_inner_from_string_f<K, C>);
	return shuffledRDD->mapToPair(xyz_pair_rdd_do_nothing_f<K, C>);
 }

/*
 * reduce data set of this PairRDD by key of Pairs
 */
template <class K, class V, class T>
PairRDD<K, V, Pair<K, V> > * PairRDD<K, V, T>::reduceByKey(
		Pair<K, V> (*reduce_function)(Pair<K, V>&, Pair<K, V>&),
		int numPartitions)
{
	return combineByKey(xyz_pair_rdd_do_nothing_f<K, V>, reduce_function, numPartitions);
}

/*
 * reduceByKey without specifying the partition number in ShuffledRDD.
 * the partition number will be the total threads count.
 */
template <class K, class V, class T>
PairRDD<K, V, Pair<K, V> > * PairRDD<K, V, T>::reduceByKey(
		Pair<K, V> (*reduce_function)(Pair<K, V>&, Pair<K, V>&))
{
	return combineByKey(
			xyz_pair_rdd_do_nothing_f<K, V>,
			reduce_function,
			(this->context)->getTotalThreads());
}

/*
 * create combiner for groupByKey.
 */
template <class K, class V>
Pair<K, VectorIteratorSeq<V> > xyz_pair_rdd_group_by_key_inner_create_combiner ( Pair<K, V> &p) {
	VectorIteratorSeq<V> iv;
	iv.push_back(p.v2);
	return Pair<K, VectorIteratorSeq<V> >(p.v1, iv);
}

/*
 * merge function used by groupByKey
 */
template <class K, class V>
Pair<K, VectorIteratorSeq<V> > xyz_pair_rdd_group_by_key_inner_merge_combiner (
		Pair<K, VectorIteratorSeq<V> > &p1,
		Pair<K, VectorIteratorSeq<V> > &p2) {
	vector<V> vv1 = p1.v2.getVector();
	vector<V> vv2 = p2.v2.getVector();
	VectorIteratorSeq<V> iv(vv1);
	iv.push_back(vv2);
	return Pair<K, VectorIteratorSeq<V> >(p1.v1, iv);
}

/*
 * groupByKey for data set in this PairRDD
 */
template <class K, class V, class T>
PairRDD<K, VectorIteratorSeq<V>, Pair<K, VectorIteratorSeq<V> > > * PairRDD<K, V, T>::groupByKey(
		int num_partitions) {
	return combineByKey(
			xyz_pair_rdd_group_by_key_inner_create_combiner<K, V>,
			xyz_pair_rdd_group_by_key_inner_merge_combiner<K, V>,
			num_partitions);

}

/*
 * groupByKey without specifying partition number of the ShuffledRDD.
 * the partition number will be the total threads count.
 */
template <class K, class V, class T>
PairRDD<K, VectorIteratorSeq<V>, Pair<K, VectorIteratorSeq<V> > > * PairRDD<K, V, T>::groupByKey() {
	return groupByKey((this->context)->getTotalThreads());

}

/*
 *  inner map function for join
 */
template <class K, class V, class W>
Pair<K, Either<V, W> > xyz_pair_rdd_join_inner_map_left_f(Pair<K, V> &pl) {
	Either<V, W> e;
	e.initLeft(pl.v2);
	return Pair<K, Either<V, W> > (pl.v1, e);
}

/*
 *  inner map function for join
 */
template <class K, class V, class W>
Pair<K, Either<V, W> > xyz_pair_rdd_join_inner_map_right_f(Pair<K, W> &pr) {
	Either<V, W> e;
	e.initRight(pr.v2);
	return Pair<K, Either<V, W> > (pr.v1, e);
}

/*
 *  inner map function for join
 */
template <class K, class V, class W>
Pair<K, Either<V, W> > xyz_pair_rdd_join_inner_map_to_pair_f(Pair<K, Either<V, W> > &p) {
	return p;
}

/*
 *  inner flat map function for join
 */
template <class K, class V, class W>
vector< Pair< K, Pair< V, W > > > xyz_pair_rdd_join_inner_flat_map_f(Pair< K, VectorIteratorSeq< Either< V, W > > > &ps) {
	vector< Pair< K, Pair< V, W > > > ret;
	vector<V> vv;
	vector<W> vw;
	for (size_t i=0; i<ps.v2.size(); i++) {
		Either< V, W > ei = ps.v2.at(i);
		if (ei.type == EITHER_TYPE_LEFT) {
			vv.push_back(ei.left);
		} else if (ei.type == EITHER_TYPE_RIGHT) {
			vw.push_back(ei.right);
		}
	}
	// create pair for each left and right values in IteratorSeq< Either< V, W> >
	for (unsigned int i=0; i<vv.size(); i++) {
		for (unsigned int j=0; j<vw.size(); j++) {
			Pair< V, W > right(vv[i], vw[j]);
			ret.push_back(Pair< K, Pair< V, W > >(ps.v1, right));
		}
	}

	return ret;
}

/*
 * to join RDD< Pair< K, V > > and RDD< Pair< K, W > >
 * return PairRDD< K, Pair< V, W> >
 */
template <class K, class V, class T>
template <class W>
PairRDD< K, Pair< V, W >, Pair< K, Pair< V, W > > > * PairRDD<K, V, T>::join(
		RDD< Pair< K, W > > *other,
		int num_partitions) {
	MappedRDD< Pair<K, Either<V, W> >, Pair<K, V> > *mapRDD1 = this->map(xyz_pair_rdd_join_inner_map_left_f<K, V, W>);
	MappedRDD< Pair<K, Either<V, W> >, Pair<K, W> > *mapRDD2 = other->map(xyz_pair_rdd_join_inner_map_right_f<K, V, W>);

	UnionRDD< Pair<K, Either<V, W> > > *all = mapRDD1->unionRDD(mapRDD2);
	return all->mapToPair(xyz_pair_rdd_do_nothing_f< K, Either<V, W> >)
			->groupByKey(num_partitions)
			->flatMap(xyz_pair_rdd_join_inner_flat_map_f<K, V, W>)
			->mapToPair(xyz_pair_rdd_do_nothing_f< K, Pair< V, W > >);

}

/*
 * to join two RDD without specifying partition number of new ShuffledRDD.
 * the partition number will be the total threads count.
 */
template <class K, class V, class T>
template <class W>
PairRDD< K, Pair< V, W >, Pair< K, Pair< V, W > > > * PairRDD<K, V, T>::join(
		RDD< Pair< K, W > > *other) {
	return join(other, (this->context)->getTotalThreads());
}

#endif /* PIARRDD_HPP_ */


