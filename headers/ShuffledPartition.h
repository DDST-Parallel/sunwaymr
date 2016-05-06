/*
 * ShuffledPartition.h
 *
 *  Created on: 2016年2月23日
 *      Author: knshen
 */

#ifndef HEADERS_SHUFFLEDPARTITION_H_
#define HEADERS_SHUFFLEDPARTITION_H_

#include "IteratorSeq.h"
#include "Partition.h"

#include <vector>
using namespace std;

/*
 * Partition of ShuffledRDD
 */
class ShuffledPartition: public Partition {
public:
	ShuffledPartition(long _rddID, int _partitionID);

	long rddID;
	int partitionID;
};


#endif /* HEADERS_SHUFFLEDPARTITION_H_ */
