/*
 * HashDivider.hpp
 *
 *  Created on: 2016年2月23日
 *      Author: knshen
 */

#ifndef INCLUDE_HASHDIVIDER_HPP_
#define INCLUDE_HASHDIVIDER_HPP_

#include "HashDivider.h"

/*
 * constructor
 */
HashDivider::HashDivider(int partitions)
{
	numPartitions = partitions;
}

/*
 * get the total number of partitions
 */
int HashDivider::getNumPartitions()
{
	return numPartitions;
}

/*
 * get the new partition index for a hash code
 */
int HashDivider::getPartition(long hashcode)
{
	int mod = hashcode % numPartitions;
	if(mod < 0)
		return mod + numPartitions;

	return mod;
}

/*
 * to determine the equality of two HashDividers
 */
bool HashDivider::equals(HashDivider hd)
{
	return numPartitions == hd.getNumPartitions();
}

#endif /* INCLUDE_HASHDIVIDER_HPP_ */
