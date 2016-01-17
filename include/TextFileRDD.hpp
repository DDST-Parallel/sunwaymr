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
#include "FileSource.hpp"
#include "AllNodesRDD.hpp"
#include "Utils.hpp"
using namespace std;

TextFileRDD::TextFileRDD(SunwayMRContext &c, vector<FileSource> files, int numSlices)
: RDD<TextFileBlock>::RDD (c), files(files), numSlices(numSlices) {
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
	TextFilePartition &partition = dynamic_cast<TextFilePartition&>(p);
	return partition.blockLocations;
}

IteratorSeq<TextFileBlock> TextFileRDD::iteratorSeq(Partition &p) {
	TextFilePartition &pap = dynamic_cast<TextFilePartition&>(p);
	return pap.iteratorSeq();
}

string master_ip;
int scheduler_listen_port;
FileSource getFileSize(void *file) {
	FileSource* fs = (FileSource*)file;
	FileSource fs_ret(*fs);
	string selfIP = getLocalHost();

	if(fs->source=="*" || fs->source==selfIP) {
		fs->location = selfIP;
		fs->listenPort = scheduler_listen_port;
		long length;
		if(getFileLength(fs->path, length)) {
			fs->length = length;
		}
	} else if (fs->source=="[DFS-server]" && selfIP==master_ip) {
		// TODO DFS file

	}

	return fs_ret;
}


vector< IteratorSeq<TextFileBlock>* > TextFileRDD::slice() {
	vector<void *> seq;
	for(unsigned int i=0; i<files.size(); i++) {
		seq.push_back(&files[i]);
	}
	master_ip = RDD<TextFileBlock>::context.master;
	scheduler_listen_port = RDD<TextFileBlock>::context.listenPort;
	// collect all file sizes
	vector<FileSource> fileSources = RDD<TextFileBlock>::context.allNodes(seq).map(getFileSize)).collect();

	long total_length = 0;
	for(unsigned int i=0; i<fileSources.size(); i++) {
		total_length += fileSources[i].length;
	}

	// create file blocks, slice
	vector< IteratorSeq<TextFileBlock>* > ret;
	if(total_length > 0) {
		// calculate block length
		long b = total_length / numSlices;
		if (b == 0) b = 1;
		else if (b > MAX_TEXT_FILE_BLOCK_SIZE) b = MAX_TEXT_FILE_BLOCK_SIZE;

		int allBlocksCountApprox = total_length / b + 1 + fileSources.size();
		int partitionBlocksCountApprox = allBlocksCountApprox / numSlices + 1;

		// get all blocks
		vector< vector<TextFileBlock> > allBlocks;
		for(unsigned int i=0; i<fileSources.size(); i++) {
			FileSource fs = fileSources[i];
			vector<TextFileBlock> fileBlocks;
			long l1 = 0;
			if (fs.length > 0) {
				int fileBlocksCount = fs.length / b;
				if(fs.length % b > 0) fileBlocksCount++;

				while(true) {
					if(l1+b <= fs.length) {
						TextFileBlock block = TextFileBlock(fs, fs.location, l1, b);
						fileBlocks.push_back(block);
						l1 += b;
					} else {
						TextFileBlock block = TextFileBlock(fs, fs.location, l1, fs.length-l1);
						fileBlocks.push_back(block);
						break;
					}
				}
				allBlocks.push_back(fileBlocks);
			}
		}

		// TODO
		// slice blocks




	} else {
		Logging::logWarning("TextFileRDD: total file length is 0.");
	}

	return ret;
}


#endif /* TEXTFILERDD_HPP_ */


