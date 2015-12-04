/*
 * Partition.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef PARTITION_H_
#define PARTITION_H_

class Partition {
public:
	Partition(int partitionID);
	virtual ~Partition();
	virtual int getPartitionID();

	int partitionID;
};

#endif /* PARTITION_H_ */
