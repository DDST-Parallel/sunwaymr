/*
 * TextFileRDD.h
 *
 *  Created on: Jan 13, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_TEXTFILERDD_H_
#define HEADERS_TEXTFILERDD_H_

#include <vector>
#include <string>

#include "IteratorSeq.h"
#include "RDD.h"
#include "SunwayMRContext.h"
#include "TextFilePartition.h"
#include "TextFileBlock.h"
using std::vector;
using std::string;

template <class T> class RDD;
class SunwayMRContext;

class TextFileRDD : public RDD<string> {
	TextFileRDD(SunwayMRContext &c, string path, string source, int numSlices);
	vector<Partition*> getPartitions();
	vector<string> preferredLocations(Partition &p);
	IteratorSeq<TextFileBlock> iteratorSeq(Partition &p);
	vector< IteratorSeq<TextFileBlock>* > slice();
	//data
	IteratorSeq<TextFileBlock> &seq;
	string path, source;
	int numSlices;
	long textFileRDD_id;

};


#endif /* HEADERS_TEXTFILERDD_H_ */
