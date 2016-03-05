/*
 * HashDivider.h
 *
 * Get partition index by hashCode.
 * Pair value will be on that partition after shuffle.
 *
 *  Created on: 2016年2月23日
 *      Author: knshen
 */

#ifndef HEADERS_HASHDIVIDER_H_
#define HEADERS_HASHDIVIDER_H_


class HashDivider
{
public:
	HashDivider(int partitions);
	int getNumPartitions();
	int getPartition(long hashcode);
	bool equals(HashDivider hd);

private:
	int numPartitions;
};



#endif /* HEADERS_HASHDIVIDER_H_ */
