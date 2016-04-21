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

TextFileBlock::TextFileBlock()
: file(FileSource()), location(""), offset(0), length(0), format(FILE_SOURCE_FORMAT_BYTE) {

}

TextFileBlock::TextFileBlock(FileSource file, string location, int offset, int length, FileSourceFormat format)
: file(file), location(location), offset(offset), length(length), format(format) {

}

TextFileBlock::TextFileBlock(const TextFileBlock &tfb)
: file(tfb.file), location(tfb.location), offset(tfb.offset), length(tfb.length), format(tfb.format) {

}

void TextFileBlock::messageReceived(int localListenPort, string fromHost, int msgType, string &msg) {

}

string TextFileBlock::blockData() {
	// retrieve data
	// TODO check local file system

	string ret;
	if (file.source == "[DFS server]") {
		// TODO DFS file
	} else {
		if(false && location == getLocalHost()) { // local file
			if (format == FILE_SOURCE_FORMAT_BYTE) {
				readFile(file.path, offset, length, ret);
			} else {
				readFileByLineNumber(file.path, offset, length, ret);
			}
		} else {
			// send file block request
			stringstream request;
			request << file.path << FILE_BLOCK_REQUEST_DELIMITATION
					<< offset << FILE_BLOCK_REQUEST_DELIMITATION
					<< length << FILE_BLOCK_REQUEST_DELIMITATION
					<< format;
			string msg = request.str();
			sendMessageForReply(location, file.listenPort,
					FILE_BLOCK_REQUEST, msg, ret);
		}
	}

	// TODO save ret to local file system

	return ret;
}

#endif

