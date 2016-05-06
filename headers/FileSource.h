/*
 * FileSource.h
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

/*
 * reading file by line or by byte
 */
enum FileSourceFormat {
	FILE_SOURCE_FORMAT_BYTE,
	FILE_SOURCE_FORMAT_LINE
};

/*
 * File source info.
 */
class FileSource {
public:
	FileSource();
	FileSource(string source, string path,
			FileSourceFormat format = FILE_SOURCE_FORMAT_BYTE);
	FileSource(const FileSource &f);

	string source, path;  // source: */./[IP]/[DFS server]
	long length; // length of file. either in lines or in bytes
	int listenPort; // listen port for receiving fetching requests
	string location; // will be used to initialize TextFileBlock
	FileSourceFormat format; // 0: byte, 1: line
	long bytes, lines; // file size
};


#endif /* HEADERS_FILESOURCE_H_ */
