/*
 * ShuffledRDDPartition.h
 *
 * Partition of ShuffledRDD
 *
 *  Created on: 2016年2月23日
 *      Author: knshen
 */

#ifndef HEADERS_SHUFFLEDRDDPARTITION_H_
#define HEADERS_SHUFFLEDRDDPARTITION_H_

#include "IteratorSeq.h"
#include "Partition.h"

#include <vector>
using namespace std;

class ShuffledRDDPartition: public Partition {
public:
	ShuffledRDDPartition(long _rddID, int _partitionID);
	//IteratorSeq<T> iteratorSeq();

	long rddID;
	int partitionID;
	//IteratorSeq<T> &values;
};


#endif /* HEADERS_SHUFFLEDRDDPARTITION_H_ */
