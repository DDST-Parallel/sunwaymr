/*
 * FileSource.hpp
 *
 *  Created on: Jan 17, 2016
 *      Author: yupeng
 */

#ifndef FILESOURCE_HPP_
#define FILESOURCE_HPP_

#include "FileSource.h"

#include <sstream>
#include <vector>
#include <string>
#include "Utils.hpp"
using std::stringstream;
using std::vector;
using std::string;

FileSource::FileSource() {
	source = "";
	path = "";
	length = 0;
	listenPort = 0;
	location = " ";
}

FileSource::FileSource(string source, string path)
: source(source), path(path) {
	length = 0;
	listenPort = 0;
	location = " ";
}

FileSource::FileSource(const FileSource &f)
: source(f.source), path(f.path) {
	length = f.length;
	listenPort = f.listenPort;
	location = f.location;
}

string FileSource::serialize(string delim) {
	stringstream ss;
	ss << source << delim << path << delim << length << delim << listenPort << delim << location;
	return ss.str();
}

void FileSource::deserialize(string s, string delim) {
	vector<string> vs;
	splitString(s, vs, delim);
	if(vs.size()>=5) {
		source = vs[0];
		path = vs[1];
		length = atoi(vs[2].c_str());
		listenPort = atoi(vs[3].c_str());
		location = vs[4];
	}
}

ostream& operator<< (ostream &out, FileSource &fs) {
	string s = fs.serialize(FILE_SOURCE_DELIMITATION);
	out << s;
	return out;
}

istream& operator>> (istream &in, FileSource &fs) {
	std::stringstream buffer;
	buffer << in.rdbuf();
	string s(buffer.str());
	fs.deserialize(s, FILE_SOURCE_DELIMITATION);
	return in;
}


#endif /* FILESOURCE_HPP_ */

