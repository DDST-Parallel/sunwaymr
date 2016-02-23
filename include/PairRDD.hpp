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
#include "IteratorSeq.hpp"
#include "Partition.hpp"
#include "RDD.hpp"
#include "Pair.hpp"
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

#endif /* PIARRDD_HPP_ */


