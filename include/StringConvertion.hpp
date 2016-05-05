/*
 * StringConvertion.hpp
 *
 *  Created on: May 5, 2016
 *      Author: yupeng
 */

#ifndef INCLUDE_STRINGCONVERTION_HPP_
#define INCLUDE_STRINGCONVERTION_HPP_

#include <string>

#include "Utils.hpp"
#include "IteratorSeq.hpp"
#include "VectorIteratorSeq.hpp"
#include "Pair.hpp"
#include "Either.hpp"
#include "FileSource.hpp"
using namespace std;

/// to_string

string to_string(const bool v) {
	if(v) return "true";
	else return "false";
}

string to_string(const char v) {
	return string(1, v);
}

string to_string(const signed char v) {
	return string(1, v);
}

string to_string(const unsigned char v) {
	return string(1, v);
}

string to_string(const short v) {
	char buffer[33];
	snprintf(buffer, sizeof(buffer), "%hd", v);
	return buffer;
}

string to_string(const unsigned short v) {
	char buffer[33];
	snprintf(buffer, sizeof(buffer), "%hu", v);
	return buffer;
}

string to_string(const int v) {
	char buffer[33];
	snprintf(buffer, sizeof(buffer), "%d", v);
	return buffer;
}

string to_string(const unsigned int v) {
	char buffer[33];
	snprintf(buffer, sizeof(buffer), "%u", v);
	return buffer;
}

string to_string(const long v) {
	char buffer[33];
	snprintf(buffer, sizeof(buffer), "%ld", v);
	return buffer;
}

string to_string(const long long v) {
	char buffer[33];
	snprintf(buffer, sizeof(buffer), "%lld", v);
	return buffer;
}

string to_string(const unsigned long v) {
	char buffer[33];
	snprintf(buffer, sizeof(buffer), "%lu", v);
	return buffer;
}

string to_string(const unsigned long long v) {
	char buffer[33];
	snprintf(buffer, sizeof(buffer), "%llu", v);
	return buffer;
}

string to_string(const float v) {
	char buffer[33];
	snprintf(buffer, sizeof(buffer), "%f", v);
	return buffer;
}

string to_string(const double v) {
	char buffer[33];
	snprintf(buffer, sizeof(buffer), "%lf", v);
	return buffer;
}

string to_string(const long double v) {
	char buffer[33];
	snprintf(buffer, sizeof(buffer), "%Lf", v);
	return buffer;
}

string to_string(const string v) {
	return v;
}

string to_string(char *v) {
	return v;
}

template <class T>
string to_string(const IteratorSeq<T> &s) {
	string ret = "";
	for(size_t i=0; i<s.size(); i++) {
		ret += ITERATORSEQ_DELIMITATION_LEFT;
		ret += to_string(s.at(i));
		ret += ITERATORSEQ_DELIMITATION_RIGHT;
	}
	return ret;
}

template <class K, class V>
string to_string(const Pair<K, V> &p) {
	string ret = "";
	ret += PAIR_DELIMITATION_LEFT;
	ret += to_string(p.v1);
	ret += PAIR_DELIMITATION_RIGHT;
	ret += PAIR_DELIMITATION_LEFT;
	ret += to_string(p.v2);
	ret += PAIR_DELIMITATION_RIGHT;
	return ret;
}


template <class L, class R>
string to_string(const Either<L, R> &e) {
	string ret = "";
	if (e.type == EITHER_TYPE_LEFT || e.type == EITHER_TYPE_RIGHT) {
		if (e.type == EITHER_TYPE_LEFT) {
			ret += EITHER_DELIMITATION_LEFT;
			ret += "LEFT";
			ret += EITHER_DELIMITATION_RIGHT;
			ret += EITHER_DELIMITATION_LEFT;
			ret += to_string(e.left);
			ret += EITHER_DELIMITATION_RIGHT;
		} else if (e.type == EITHER_TYPE_RIGHT) {
			ret += EITHER_DELIMITATION_LEFT;
			ret += "RIGHT";
			ret += EITHER_DELIMITATION_RIGHT;
			ret += EITHER_DELIMITATION_LEFT;
			ret += to_string(e.right);
			ret += EITHER_DELIMITATION_RIGHT;
		}
	} else {
		ret += EITHER_DELIMITATION_LEFT;
		ret += "NA";
		ret += EITHER_DELIMITATION_RIGHT;
	}

	return ret;
}

string to_string(const FileSource &fs) {
	string ret = "";
	ret = ret + fs.source
			+ FILE_SOURCE_DELIMITATION
			+ fs.path + FILE_SOURCE_DELIMITATION
			+ to_string(fs.length)
			+ FILE_SOURCE_DELIMITATION
			+ to_string(fs.listenPort)
			+ FILE_SOURCE_DELIMITATION
			+ fs.location + FILE_SOURCE_DELIMITATION
			+ to_string(fs.format)
			+ FILE_SOURCE_DELIMITATION
			+ to_string(fs.bytes)
			+ FILE_SOURCE_DELIMITATION
			+ to_string(fs.lines);
	return ret;
}

/// from_string

void from_string(bool &v, string s) {
	v = false;
	if(s == "true") v = true;
}

void from_string(char &v, string s) {
	v = 0;
	if(s.length() > 0) v = s[0];
}

void from_string(short &v, string s) {
	v = 0;
	sscanf(s.c_str(), "%hd", &v);
}

void from_string(unsigned short &v, string s) {
	v = 0;
	sscanf(s.c_str(), "%hu", &v);
}

void from_string(int &v, string s) {
	v = 0;
	sscanf(s.c_str(), "%d", &v);
}

void from_string(unsigned int &v, string s) {
	v = 0;
	sscanf(s.c_str(), "%u", &v);
}

void from_string(long &v, string s) {
	v = 0;
	sscanf(s.c_str(), "%ld", &v);
}

void from_string(long long &v, string s) {
	v = 0;
	sscanf(s.c_str(), "%lld", &v);
}

void from_string(unsigned long &v, string s) {
	v = 0;
	sscanf(s.c_str(), "%lu", &v);
}

void from_string(unsigned long long &v, string s) {
	v = 0;
	sscanf(s.c_str(), "%llu", &v);
}

void from_string(float &v, string s) {
	v = 0;
	sscanf(s.c_str(), "%f", &v);
}

void from_string(double &v, string s) {
	v = 0;
	sscanf(s.c_str(), "%lf", &v);
}

void from_string(long double &v, string s) {
	v = 0;
	sscanf(s.c_str(), "%Lf", &v);
}

void from_string(string &v, string s) {
	v = s;
}

template <class T>
void from_string(VectorIteratorSeq<T> &s, string str) {
	vector<string> vs;
	s.clear();
	vs = splitStringByDelimitationCouple(
			str,
			ITERATORSEQ_DELIMITATION_LEFT,
			ITERATORSEQ_DELIMITATION_RIGHT);
	for (unsigned int i=0; i<vs.size(); i++) {
		T t;
		from_string(t, vs[i]);
		s.push_back(t);
	}
}

template <class K, class V>
void from_string(Pair<K, V> &p, string s) {
	vector<string> vs;
	vs = splitStringByDelimitationCouple(s, PAIR_DELIMITATION_LEFT, PAIR_DELIMITATION_RIGHT);
	if (vs.size() > 1) {
		K k;
		V v;
		from_string(k, vs[0]);
		from_string(v, vs[1]);
		p.v1 = k;
		p.v2 = v;

		p.valid = true;
	}
}

template <class L, class R>
void from_string(Either<L, R> &e, string s) {
	vector<string> vs;
	vs = splitStringByDelimitationCouple(s, EITHER_DELIMITATION_LEFT, EITHER_DELIMITATION_RIGHT);
	if (vs.size() == 2) {
		if (vs[0] == "LEFT") {
			L l;
			from_string(l, vs[1]);
			e.initLeft(l);
		} else if (vs[0] == "RIGHT"){
			R r;
			from_string(r, vs[1]);
			e.initRight(r);
		}
	}
}

void from_string(FileSource &fs, string s) {
	vector<string> vs;
	splitString(s, vs, FILE_SOURCE_DELIMITATION);
	if(vs.size()>=8) {
		fs.source = vs[0];
		fs.path = vs[1];
		from_string(fs.length, vs[2]);
		from_string(fs.listenPort, vs[3]);
		fs.location = vs[4];
		fs.format = static_cast<FileSourceFormat>(atoi(vs[5].c_str()));
		from_string(fs.bytes, vs[6]);
		from_string(fs.lines, vs[7]);
	}
}


#endif /* INCLUDE_STRINGCONVERTION_HPP_ */
