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
	format = FILE_SOURCE_FORMAT_BYTE;
	bytes = 0;
	lines = 0;
}

FileSource::FileSource(string source, string path, FileSourceFormat format)
: source(source), path(path), format(format) {
	length = 0;
	listenPort = 0;
	location = " ";
	bytes = 0;
	lines = 0;
}

FileSource::FileSource(const FileSource &f)
: source(f.source), path(f.path), format(f.format) {
	length = f.length;
	listenPort = f.listenPort;
	location = f.location;
	bytes = f.bytes;
	lines = f.lines;
}

string FileSource::serialize(string delim) const {
	stringstream ss;
	ss << source << delim << path << delim << length 
		<< delim << listenPort << delim << location << delim << format
		<< delim << bytes << delim << lines;
	return ss.str();
}

void FileSource::deserialize(string s, string delim) {
	vector<string> vs;
	splitString(s, vs, delim);
	if(vs.size()>=8) {
		source = vs[0];
		path = vs[1];
		stringstream ss1;
		ss1 << vs[2];
		ss1 >> length;
		listenPort = atoi(vs[3].c_str());
		location = vs[4];
		format = static_cast<FileSourceFormat>(atoi(vs[5].c_str()));
		stringstream ss2;
		ss2 << vs[6];
		ss2 >> bytes;
		stringstream ss3;
		ss3 << vs[7];
		ss3 >> lines;
	}
}

ostream& operator<< (ostream &out, const FileSource &fs) {
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

