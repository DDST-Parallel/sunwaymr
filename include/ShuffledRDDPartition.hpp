/*
 * ShuffledRDDPartition.hpp
 *
 *  Created on: 2016年2月23日
 *      Author: knshen
 */

#ifndef INCLUDE_SHUFFLEDRDDPARTITION_HPP_
#define INCLUDE_SHUFFLEDRDDPARTITION_HPP_


#include "ShuffledRDDPartition.h"
#include "IteratorSeq.hpp"
#include "Partition.hpp"

ShuffledRDDPartition::ShuffledRDDPartition(long _rddID, int _partitionID)
: rddID(_rddID), partitionID(_partitionID)
{
}


#endif /* INCLUDE_SHUFFLEDRDDPARTITION_HPP_ */
