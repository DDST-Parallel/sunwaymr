/*
 * TextFilePartition.hpp
 *
 *  Created on: Jan 13, 2016
 *      Author: yupeng
 */

#ifndef TEXTFILEPARTITION_HPP_
#define TEXTFILEPARTITION_HPP_

#include "TextFilePartition.h"

#include <iostream>
#include "IteratorSeq.hpp"
#include "Partition.hpp"
#include "TextFileBlock.h"
using namespace std;


TextFilePartition::TextFilePartition(long _rddID, int _partitionID, IteratorSeq<TextFileBlock> &_values)
: rddID(_rddID), partitionID(_partitionID), values(_values)
{
	for(unsigned int i=0; i<values.size(); i++) {
		TextFileBlock block = values[i];
		blockLocations.push_back(block.location);
	}
}

IteratorSeq<TextFileBlock> TextFilePartition::iteratorSeq()
{
	return values;
}

#endif /* TEXTFILEPARTITION_HPP_ */


