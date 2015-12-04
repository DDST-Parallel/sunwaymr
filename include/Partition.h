/*
 * Partition.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef PARTITION_H_
#define PARTITION_H_

#include "IteratorSeq.h"

template <class T>
class Partition {
public:
	virtual ~Partition();
	virtual IteratorSeq<T> iteratorSeq() = 0;
};

#endif /* PARTITION_H_ */
