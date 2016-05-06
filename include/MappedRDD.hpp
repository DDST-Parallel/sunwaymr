#ifndef MAPPEDRDD_HPP_
#define MAPPEDRDD_HPP_


#include "MappedRDD.h"

#include <iostream>
#include "IteratorSeq.hpp"
#include "Partition.hpp"
#include "RDD.hpp"
using namespace std;

/*
 * constructor, accepting previous RDD and mapped function pointer
 */
template <class U, class T>
MappedRDD<U, T>::MappedRDD(RDD<T> *prev, U (*f)(T&))
:RDD<U>::RDD(prev->context), prevRDD(prev)
{
	mappedFunction = f;
}

/*
 * destructor, deleting previous RDD if not sticky
 */
template <class U, class T>
MappedRDD<U, T>::~MappedRDD()
{
	if(!this->prevRDD->isSticky()) {
		delete this->prevRDD;
	}
}

/*
 * shuffle the previous RDD, this MappedRDD does not need to shuffle
 */
template <class U, class T>
void MappedRDD<U, T>::shuffle()
{
	prevRDD->shuffle();
}

/*
 * get partitions of this RDD.
 * as to MapppedRDD, all partitions are from its previous RDD.
 */
template <class U, class T>
vector<Partition*> MappedRDD<U, T>::getPartitions()
{
	return prevRDD->getPartitions();
}

/*
 * get the preferred locations of the partition.
 * mapping does not change the preferred locations of partitions
 */
template <class U, class T>
vector<string> MappedRDD<U, T>::preferredLocations(Partition *p)
{
	return prevRDD->preferredLocations(p);
}

/*
 * get the data set in the partition.
 * return the mapped IteratorSeq from previous RDD.
 */
template <class U, class T>
IteratorSeq<U> * MappedRDD<U, T>::iteratorSeq(Partition *p)
{
	IteratorSeq<U> *ret = prevRDD->iteratorSeq(p)->map(mappedFunction);
	this->addIteratorSeq(ret); // for garbage collection
	return ret;
}


#endif /* MAPPEDRDD_HPP_ */
