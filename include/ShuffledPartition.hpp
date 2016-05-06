/*
 * ShuffledPartition.hpp
 *
 *  Created on: 2016年2月23日
 *      Author: knshen
 */

#ifndef INCLUDE_SHUFFLEDPARTITION_HPP_
#define INCLUDE_SHUFFLEDPARTITION_HPP_

#include "ShuffledPartition.h"

#include "IteratorSeq.hpp"
#include "Partition.hpp"

/*
 * constructor
 */
ShuffledPartition::ShuffledPartition(long _rddID, int _partitionID)
: rddID(_rddID), partitionID(_partitionID)
{
}


#endif /* INCLUDE_SHUFFLEDPARTITION_HPP_ */
