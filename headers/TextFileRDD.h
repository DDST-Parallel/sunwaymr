/*
 * TextFileRDD.h
 *
 * Context::textFile() creates TextFileRDD.
 * TextFileRDD is constructed from vector<FileSource>.
 * TextFileRDD creates TextFileBlocks from files, and partition them into TextFilePartitions
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
#include "FileSource.h"
using std::vector;
using std::string;

template <class T> class RDD;
class SunwayMRContext;

class TextFileRDD : public RDD<TextFileBlock> {
public:
	TextFileRDD(SunwayMRContext *c, vector<FileSource> &files, int numSlices,
			FileSourceFormat format = FILE_SOURCE_FORMAT_BYTE);
	vector<Partition*> getPartitions();
	vector<string> preferredLocations(Partition *p);
	IteratorSeq<TextFileBlock> * iteratorSeq(Partition *p);
	vector< IteratorSeq<TextFileBlock>* > slice(vector<FileSource> &files);

	//data
	int numSlices;
	FileSourceFormat format;
	long textFileRDD_id;

};


#endif /* HEADERS_TEXTFILERDD_H_ */
