/*
 * FileSource.hpp
 *
 *  Created on: Jan 17, 2016
 *      Author: yupeng
 */

#ifndef FILESOURCE_HPP_
#define FILESOURCE_HPP_

#include "FileSource.h"

#include <vector>
#include <string>
#include "Utils.hpp"
using std::vector;
using std::string;

/*
 * default constructor
 */
FileSource::FileSource() {
	source = "";
	path = "";
	length = 0;
	listenPort = 0;
	location = " ";
	format = FILE_SOURCE_FORMAT_BYTE;
	bytes = 0;
	lines = 0;
}

/*
 * initializing a file source.
 * accepted source types: *|.|[IP]|[DFS server]
 *   1) *              every node has a file path, the file content may be different
 *   2) .              every node has a same copy of file content
 *   3) [IP]           the file content is at the host [IP]
 *   4) [DFS server]   the file content is in some kind of distributed file system
 */
FileSource::FileSource(string source, string path, FileSourceFormat format)
: source(source), path(path), format(format) {
	length = 0;
	listenPort = 0;
	location = " ";
	bytes = 0;
	lines = 0;
}

/*
 * copy constructor
 */
FileSource::FileSource(const FileSource &f)
: source(f.source), path(f.path), format(f.format) {
	length = f.length;
	listenPort = f.listenPort;
	location = f.location;
	bytes = f.bytes;
	lines = f.lines;
}

#endif /* FILESOURCE_HPP_ */

