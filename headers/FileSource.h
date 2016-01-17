/*
 * FileSource.h
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

class FileSource {
public:
	FileSource();
	FileSource(string source, string path); // source: */[IP]/[DFS server]
	FileSource(FileSource &f);
	string serialize(char delim);
	void deserialize(string s, char delim);
	friend ostream& operator<< (ostream &out, FileSource &fs);
	friend istream& operator>> (istream &in, FileSource &fs);

	string source, path;
	long length;
	int listenPort;
	string location; // will be used to initialize TextFileBlock
};


#endif /* HEADERS_FILESOURCE_H_ */
