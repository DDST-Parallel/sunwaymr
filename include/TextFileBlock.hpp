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
#include "Messaging.hpp"
#include "Utils.hpp"

TextFileBlock::TextFileBlock(FileSource file, string location, int offset, int length)
: file(file), location(location), offset(offset), length(length){

}

TextFileBlock::TextFileBlock(TextFileBlock &tfb)
: file(tfb.file), location(tfb.location), offset(tfb.offset), length(tfb.length){

}

void TextFileBlock::messageReceived(int localListenPort, string fromHost, int msgType, string msg) {

}

string TextFileBlock::blockData() {
	// retrieve data
	string ret;
	if (file.source == "[DFS server]") {
		// TODO DFS file
	} else {
		if(location == getLocalHost()) { // local file
			readFile(file.path, offset, length, ret);
		} else {
			// send file block request
			stringstream request;
			request << file.path << "|" << offset << "|" << length;
			sendMessageForReply(location, file.listenPort,
					FILE_BLOCK_REQUEST, request.str(), ret);
		}
	}

	return ret;
}

#endif

