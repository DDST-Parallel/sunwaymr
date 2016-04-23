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

template <class U, class T>
FlatMappedRDD<U, T>::FlatMappedRDD(RDD<T> *prev, vector<U> (*f)(T&))
:RDD<U>::RDD(prev->context), prevRDD(prev)
{
	mappedFunction = f;
}

template <class U, class T>
FlatMappedRDD<U, T>::~FlatMappedRDD()
{
	if(!this->prevRDD->isSticky()) {
		delete this->prevRDD;
	}
}

template <class U, class T>
void FlatMappedRDD<U, T>::shuffle()
{
	prevRDD->shuffle();
}

template <class U, class T>
vector<Partition *> FlatMappedRDD<U, T>::getPartitions()
{
	return prevRDD->getPartitions();
}

template <class U, class T>
vector<string> FlatMappedRDD<U, T>::preferredLocations(Partition *p)
{
	return prevRDD->preferredLocations(p);
}

template <class U, class T>
IteratorSeq<U> * FlatMappedRDD<U, T>::iteratorSeq(Partition *p)
{
	IteratorSeq<U> * ret = prevRDD->iteratorSeq(p)->flatMap(mappedFunction);
	this->addIteratorSeq(ret); // for garbage collection
	return ret;
}


#endif /* FLATMAPPEDRDD_HPP_ */



