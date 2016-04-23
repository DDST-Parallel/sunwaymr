#ifndef MAPPEDRDD_HPP_
#define MAPPEDRDD_HPP_


#include "MappedRDD.h"

#include <iostream>
#include "IteratorSeq.hpp"
#include "Partition.hpp"
#include "RDD.hpp"
using namespace std;

template <class U, class T>
MappedRDD<U, T>::MappedRDD(RDD<T> *prev, U (*f)(T&))
:RDD<U>::RDD(prev->context), prevRDD(prev)
{
	mappedFunction = f;
}

template <class U, class T>
MappedRDD<U, T>::~MappedRDD()
{
	if(!this->prevRDD->isSticky()) {
		delete this->prevRDD;
	}
}

template <class U, class T>
void MappedRDD<U, T>::shuffle()
{
	prevRDD->shuffle();
}

template <class U, class T>
vector<Partition*> MappedRDD<U, T>::getPartitions()
{
	return prevRDD->getPartitions();
}

template <class U, class T>
vector<string> MappedRDD<U, T>::preferredLocations(Partition *p)
{
	return prevRDD->preferredLocations(p);
}

template <class U, class T>
IteratorSeq<U> * MappedRDD<U, T>::iteratorSeq(Partition *p)
{
	IteratorSeq<U> *ret = prevRDD->iteratorSeq(p)->map(mappedFunction);
	this->addIteratorSeq(ret); // for garbage collection
	return ret;
}


#endif /* MAPPEDRDD_HPP_ */
