/*
 * AllNodesRDD.h
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
#include "PointerContainer.h"
using std::vector;
using std::string;

template <class T> class RDD;
template <class U, class T> class MappedRDD;
class SunwayMRContext;

/*
 * A RDD with a same partition on each node.
 * An easy way to run the same code on all nodes.
 */
template <class T>
class AllNodesRDD : public RDD<PointerContainer <T> > {
public:
	AllNodesRDD(SunwayMRContext *c, IteratorSeq<PointerContainer <T> > *seq);
	~AllNodesRDD();
	vector<Partition*> getPartitions();
	vector<string> preferredLocations(Partition *p);
	IteratorSeq<PointerContainer <T> > * iteratorSeq(Partition *p);

	IteratorSeq<PointerContainer <T> > *seq; // data
	long rdd_id;
};


#endif /* HEADERS_ALLNODESRDD_H_ */
