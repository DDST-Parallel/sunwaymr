/*
 * AllNodesRDD.hpp
 *
 *  Created on: Jan 17, 2016
 *      Author: yupeng
 */

#ifndef ALLNODESRDD_HPP_
#define ALLNODESRDD_HPP_

#include "AllNodesRDD.h"

#include <iostream>
#include <sstream>
#include <stdlib.h>

#include "IteratorSeq.hpp"
#include "RDD.hpp"
#include "Partition.hpp"
#include "SunwayMRContext.hpp"
#include "AllNodesPartition.hpp"
#include "PointerContainer.hpp"
using namespace std;

template <class T>
AllNodesRDD<T>::AllNodesRDD(SunwayMRContext *c, IteratorSeq<PointerContainer <T> > *seq)
: RDD<PointerContainer <T> >::RDD (c) , seq(seq){
	rdd_id = RDD<PointerContainer <T> >::rddID;

	vector<Partition *> partitions;
	vector<string> hosts = c->getHosts();
	for (unsigned int i=0; i<hosts.size(); i++) {
		Partition* partition = new AllNodesPartition<T>(rdd_id, i, seq);
		partitions.push_back(partition);
	}

	RDD<PointerContainer <T> >::partitions = partitions;
}

template <class T>
AllNodesRDD<T>::~AllNodesRDD() {
//	for(size_t i = 0; i < seq->size(); i++) {
//		delete seq->at(i); // undefined
//	}
//	delete seq;
}

template <class T>
vector<Partition*> AllNodesRDD<T>::getPartitions() {
	return RDD<PointerContainer <T> >::partitions;
}

template <class T>
vector<string> AllNodesRDD<T>::preferredLocations(Partition *p) {
	vector<string> ret;

	vector<string> hosts = RDD<PointerContainer <T> >::context->getHosts();
	AllNodesPartition<T> *partition = dynamic_cast<AllNodesPartition<T> *>(p);
	ret.push_back(hosts[partition->partitionID]);

	return ret;
}

template <class T>
IteratorSeq<PointerContainer <T> > * AllNodesRDD<T>::iteratorSeq(Partition *p) {
	AllNodesPartition<T> *partition = dynamic_cast<AllNodesPartition<T> *>(p);
	return partition->iteratorSeq();
}

#endif /* ALLNODESRDD_HPP_ */

