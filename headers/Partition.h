/*
 * Partition.h
 *
 * Base class of RDD partition.
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef PARTITION_H_
#define PARTITION_H_


class Partition {
public:
	virtual ~Partition();
	virtual void printInfo();

};

#endif /* PARTITION_H_ */
