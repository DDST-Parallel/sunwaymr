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

class FileSource {
public:
	FileSource(string source, string path); // source: */[IP]/[DFS server]
	FileSource(FileSource &f);

	string source, path;
};


#endif /* HEADERS_FILESOURCE_H_ */
