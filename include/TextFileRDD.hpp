/*
 * TextFileRDD.hpp
 *
 *  Created on: Jan 13, 2016
 *      Author: yupeng
 */

#ifndef TEXTFILERDD_HPP_
#define TEXTFILERDD_HPP_

#include "TextFileRDD.h"

#include <iostream>
#include <sstream>
#include <stdlib.h>

#include "IteratorSeq.hpp"
#include "RDD.hpp"
#include "Partition.hpp"
#include "SunwayMRContext.hpp"
#include "TextFilePartition.hpp"
#include "TextFileBlock.hpp"
using namespace std;

TextFileRDD::TextFileRDD(SunwayMRContext &c, string path, string source, int numSlices)
: RDD<TextFileBlock>::RDD (c), path(path), source(source), numSlices(numSlices) {
	textFileRDD_id = RDD<TextFileBlock>::current_id++;

	// calculate partitions
	vector<Partition*> partitions;
	vector< IteratorSeq<TextFileBlock>* > slices = slice();

	if(slices.size() == 0) {
		exit(104);
	}

	//construct partitions
	for (int i = 0; i < slices.size(); i++)
	{
		Partition* partition = new TextFilePartition(textFileRDD_id, i, *slices[i]);
		partitions.push_back(partition);
	}

	RDD<TextFileBlock>::partitions = partitions;
}


vector<Partition*> TextFileRDD::getPartitions() {
	return RDD<TextFileBlock>::partitions;
}

vector<string> TextFileRDD::preferredLocations(Partition &p) {
	vector<string> ret;
	// TODO

	return ret;
}

IteratorSeq<TextFileBlock> TextFileRDD::iteratorSeq(Partition &p) {
	TextFilePartition &pap = dynamic_cast<TextFilePartition&>(p);
	return pap.iteratorSeq();
}

vector< IteratorSeq<TextFileBlock>* > TextFileRDD::slice() {
	// TODO

}


#endif /* TEXTFILERDD_HPP_ */


