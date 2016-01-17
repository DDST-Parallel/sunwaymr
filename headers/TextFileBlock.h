/*
 * TextFileBlock.h
 *
 *  Created on: Jan 15, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_TEXTFILEBLOCK_H_
#define HEADERS_TEXTFILEBLOCK_H_

#include <string>
#include "FileSource.h"
using std::string;


class TextFileBlock {
public:
	TextFileBlock(FileSource file, string location, int offset, int length);
	string blockData(); // retrieve file data

	FileSource file;
	string location;
	int offset, length;


};

#endif /* HEADERS_TEXTFILEBLOCK_H_ */
