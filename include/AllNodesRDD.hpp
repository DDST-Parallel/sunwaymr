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
using namespace std;

AllNodesRDD::AllNodesRDD(SunwayMRContext &c, IteratorSeq<void *> &seq)
: RDD<void *>::RDD (c) , seq(seq){
	rdd_id = RDD<void *>::current_id++;

	vector<Partition*> partitions;
	vector<string> hosts = c.getHosts();
	for (unsigned int i=0; i<hosts.size(); i++) {
		Partition* partition = new AllNodesPartition(rdd_id, i, seq);
		partitions.push_back(partition);
	}

	RDD<void *>::partitions = partitions;
}

vector<Partition*> AllNodesRDD::getPartitions() {
	return RDD<void *>::partitions;
}

vector<string> AllNodesRDD::preferredLocations(Partition &p) {
	vector<string> ret;

	vector<string> hosts = RDD<void *>::context.getHosts();
	AllNodesPartition &partition = dynamic_cast<AllNodesPartition&>(p);
	ret.push_back(hosts[partition.partitionID]);

	return ret;
}

IteratorSeq<void *> AllNodesRDD::iteratorSeq(Partition &p) {
	AllNodesPartition &partition = dynamic_cast<AllNodesPartition&>(p);
	return partition.iteratorSeq();
}

#endif /* ALLNODESRDD_HPP_ */

