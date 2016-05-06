/*
 * FlatMapppedRDD.hpp
 *
 *  Created on: Jan 13, 2016
 *      Author: yupeng
 */

#ifndef FLATMAPPEDRDD_HPP_
#define FLATMAPPEDRDD_HPP_


#include "FlatMappedRDD.h"

#include <iostream>
#include "IteratorSeq.hpp"
#include "Partition.hpp"
#include "RDD.hpp"
#include "Utils.hpp"
using namespace std;

/*
 * constructor
 */
template <class U, class T>
FlatMappedRDD<U, T>::FlatMappedRDD(RDD<T> *prev, vector<U> (*f)(T&))
:RDD<U>::RDD(prev->context), prevRDD(prev)
{
	mappedFunction = f;
}

/*
 * destructor
 */
template <class U, class T>
FlatMappedRDD<U, T>::~FlatMappedRDD()
{
	if(!this->prevRDD->isSticky()) {
		delete this->prevRDD; // if the previous RDD is not sticky, delete it
	}
}

/*
 * do shuffling.
 * only ShuffledRDD need to shuffle data set in each partition.
 * as to FlatMappedRDD, there is no work to do.
 * just invoke the previous RDD's shuffle function.
 */
template <class U, class T>
void FlatMappedRDD<U, T>::shuffle()
{
	prevRDD->shuffle();
}

/*
 * get partitions of the RDD.
 * as to FlatMapppedRDD, partitions are from its previous RDD.
 */
template <class U, class T>
vector<Partition *> FlatMappedRDD<U, T>::getPartitions()
{
	return prevRDD->getPartitions();
}

/*
 * get preferred locations for the partition.
 * as to FlatMappedRDD, the partition is from its previous RDD.
 * so, just invoke previous RDD's preferredLocaitons function.
 */
template <class U, class T>
vector<string> FlatMappedRDD<U, T>::preferredLocations(Partition *p)
{
	return prevRDD->preferredLocations(p);
}

/*
 * get the data set in the partition.
 * return the flat mapped IteratorSeq from previous RDD.
 */
template <class U, class T>
IteratorSeq<U> * FlatMappedRDD<U, T>::iteratorSeq(Partition *p)
{
	IteratorSeq<U> * ret = prevRDD->iteratorSeq(p)->flatMap(mappedFunction);
	this->addIteratorSeq(ret); // for garbage collection
	return ret;
}


#endif /* FLATMAPPEDRDD_HPP_ */



