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
#include "Utils.hpp"
using std::stringstream;

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

FileSource::FileSource(FileSource &f)
: source(f.source), path(f.path) {
	length = 0;
	listenPort = 0;
	location = " ";
}

string FileSource::serialize(char delim) {
	stringstream ss;
	ss << source << delim << path << delim << length << delim << listenPort << delim << location;
	return ss.str();
}

void FileSource::deserialize(string s, char delim) {
	vector<string> vs = splitString(s, delim);
	if(vs.size()>=5) {
		source = vs[0];
		path = vs[1];
		length = atoi(vs[2].c_str());
		listenPort = atoi(vs[3].c_str());
		location = vs[4];
	}
}

ostream& operator<< (ostream &out, FileSource &fs) {
	string s = fs.serialize('|');
	out << s;
	return out;
}

istream& operator>> (istream &in, FileSource &fs) {
	string s;
	in >> s;
	fs.deserialize(s, '|');
	return in;
}


#endif /* FILESOURCE_HPP_ */

