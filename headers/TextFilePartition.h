/*
 * TextFilePartition.h
 *
 *  Created on: Jan 13, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_TEXTFILEPARTITION_H_
#define HEADERS_TEXTFILEPARTITION_H_

#include <vector>
#include <string>

#include "IteratorSeq.h"
#include "Partition.h"
#include "TextFileBlock.h"
using std::vector;
using std::string;

class TextFilePartition: public Partition {
public:
	TextFilePartition(long rddID, int partitionID, IteratorSeq<TextFileBlock> &values);
	IteratorSeq<TextFileBlock> iteratorSeq();

	long rddID;
	int partitionID;
	IteratorSeq<TextFileBlock> &values;
	vector<string> blockLocations;
};


#endif /* HEADERS_TEXTFILEPARTITION_H_ */
