/*
 * TextFileBlock.h
 *
 *  Created on: Jan 15, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_TEXTFILEBLOCK_H_
#define HEADERS_TEXTFILEBLOCK_H_

class TextFileBlock {
public:
	TextFileBlock(string path, string source, int offset, int length);
	string blockData(); // retrieve file data

	string path, source;
	int offset, length;

};

#endif /* HEADERS_TEXTFILEBLOCK_H_ */
