/*
 * AllNodesRDD.h
 *
 * A RDD with an partition on each node.
 * An easy way to run same code on all nodes.
 *
 *  Created on: Jan 17, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_ALLNODESRDD_H_
#define HEADERS_ALLNODESRDD_H_

#include <vector>
#include <string>

#include "IteratorSeq.h"
#include "AllNodesPartition.h"
#include "RDD.h"
#include "SunwayMRContext.h"
using std::vector;
using std::string;

template <class T> class RDD;
template <class U, class T> class MappedRDD;
class SunwayMRContext;

class AllNodesRDD : public RDD<void *> {
public:
	AllNodesRDD(SunwayMRContext &c, IteratorSeq<void *> &seq);
	vector<Partition*> getPartitions();
	vector<string> preferredLocations(Partition &p);
	IteratorSeq<void *> iteratorSeq(Partition &p);

	//data
	IteratorSeq<void *> &seq;
	long rdd_id;
};


#endif /* HEADERS_ALLNODESRDD_H_ */
