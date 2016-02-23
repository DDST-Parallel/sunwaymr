/*
 * HashDivider.hpp
 *
 *  Created on: 2016年2月23日
 *      Author: knshen
 */

#ifndef INCLUDE_HASHDIVIDER_HPP_
#define INCLUDE_HASHDIVIDER_HPP_

#include "HashDivider.h"

HashDivider::HashDivider(int partitions)
{
	numPartitions = partitions;
}

int HashDivider::getNumPartitions()
{
	return numPartitions;
}

int HashDivider::getPartition(long hashcode)
{
	int mod = hashcode % numPartitions;
	if(mod < 0)
		return mod + numPartitions;

	return mod;
}

bool HashDivider::equals(HashDivider hd)
{
	return numPartitions == hd.getNumPartitions();
}

#endif /* INCLUDE_HASHDIVIDER_HPP_ */
