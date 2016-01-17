/*
 * AllNodesPartition.hpp
 *
 *  Created on: Jan 17, 2016
 *      Author: yupeng
 */

#ifndef ALLNODESPARTITION_HPP_
#define ALLNODESPARTITION_HPP_

#include "AllNodesPartition.h"

#include <iostream>
#include "IteratorSeq.hpp"
#include "Partition.hpp"
using namespace std;

AllNodesPartition::AllNodesPartition(long _rddID, int _partitionID, IteratorSeq<void *> &_values)
: rddID(_rddID), partitionID(_partitionID), values(_values)
{

}

IteratorSeq<void *> AllNodesPartition::iteratorSeq()
{
	return values;
}

#endif /* ALLNODESPARTITION_HPP_ */

