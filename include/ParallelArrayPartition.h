/*
 * ParallelArrayPartition.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef PARALLELARRAYPARTITION_H_
#define PARALLELARRAYPARTITION_H_

#include <vector>
#include "Partition.h"
using std::vector;

template <class T>
class ParallelArrayPartition: public Partition {
public:
	ParallelArrayPartition(long rddID, int partitionID, vector<T> values);
	vector<T> iteratorArray();

private:
	long rddID;
	int partitionID;
	vector<T> values;
};


#endif /* PARALLELARRAYPARTITION_H_ */
