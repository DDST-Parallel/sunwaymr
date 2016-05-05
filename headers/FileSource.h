/*
 * FileSource.h
 *
 * File source info.
 *
 *  Created on: Jan 17, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_FILESOURCE_H_
#define HEADERS_FILESOURCE_H_

#include <string>
using std::string;

#ifndef FILE_SOURCE_DELIMITATION
#define FILE_SOURCE_DELIMITATION "\aFILE_SOURCE\a"
#endif

enum FileSourceFormat {
	FILE_SOURCE_FORMAT_BYTE,
	FILE_SOURCE_FORMAT_LINE
};

class FileSource {
public:
	FileSource();
	FileSource(string source, string path, FileSourceFormat format = FILE_SOURCE_FORMAT_BYTE); // source: */[IP]/[DFS server]
	FileSource(const FileSource &f);

	string source, path;
	long length;
	int listenPort;
	string location; // will be used to initialize TextFileBlock
	FileSourceFormat format; // 0: byte, 1: line
	long bytes, lines;
};


#endif /* HEADERS_FILESOURCE_H_ */
