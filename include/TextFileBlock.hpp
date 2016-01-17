/*
 * TextFileBlock.hpp
 *
 *  Created on: Jan 15, 2016
 *      Author: yupeng
 */

#ifndef TEXTFILEBLOCK_HPP_
#define TEXTFILEBLOCK_HPP_

#include "TextFileBlock.h"

#include "FileSource.hpp"

TextFileBlock::TextFileBlock(FileSource file, string location, int offset, int length)
: file(file), location(location), offset(offset), length(length) {

}

string TextFileBlock::blockData() {
	// TODO retrieve data

}

#endif

