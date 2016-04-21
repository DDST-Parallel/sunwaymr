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
#include <iostream>
using std::string;
using std::ostream;
using std::istream;

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
	string serialize(string delim) const;
	void deserialize(string &s, string delim);
	friend ostream& operator<< (ostream &out, const FileSource &fs);
	friend istream& operator>> (istream &in, FileSource &fs);

	string source, path;
	long length;
	int listenPort;
	string location; // will be used to initialize TextFileBlock
	FileSourceFormat format; // 0: byte, 1: line
	long bytes, lines;
};


#endif /* HEADERS_FILESOURCE_H_ */
