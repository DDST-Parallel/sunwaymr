/*
 * SunwayMRWordCount.cpp
 *
 *  Created on: Jan 28, 2016
 *      Author: yupeng
 */

#include <iostream>
#include <vector>
#include <sstream>
#include "SunwayMRContext.hpp"
#include "FileSource.hpp"
#include "TextFileBlock.hpp"
#include "Pair.hpp"
#include "Utils.hpp"
using std::cout;
using std::endl;
using std::vector;
using std::stringstream;

vector<string> flatMap_f(TextFileBlock t) {
	vector<string> ret;
	string data = t.blockData();
	stringstream ss(data);
	string word;
	while (ss >> word) {
		ret.push_back(word)
	}
	return ret;
}

Pair<string, int> mapToPair_f(string s) {
	return Pair<string, int>(s, 1);
}

Pair<string, int> merge_f (Pair<string, int> p1, Pair<string, int> p2) {
	return Pair<string, int>(p1.v1, p1.v2+p2.v2);
}

long hash_f (Pair<string, int> p) {
	return p.v1.length(); // TODO
}

#ifndef PAIR_DELIMITATION
#define PAIR_DELIMITATION "\aPAIR\a";
#endif
string toStr_f (Pair<string, int> p) {
	stringstream ss;
	ss << p.v1 << PAIR_DELIMITATION << p.v2;
	return ss.str();
}

Pair<string, int> fromStr_f (string s) {
	vector<string> vs;
	splitString(s, vs, PAIR_DELIMITATION);
	int i = atoi(vs[1].c_str());
	return Pair<string, int>(vs[0], i);
}

int main(int argc, char *argv[]) {

	cout << "SunwayMR Word Count Totally" << endl;

	SunwayMRContext sc("SunwayMRWordCountTotally", argc, argv);
	vector<FileSource> fsv;
	FileSource fs = FileSource("192.168.1.66", "/tmp/1.txt");
	fsv.push_back(fs);
	vector< Pair<string, int> > wc = sc.textFile(fsv)
			.flatMap(flatMap_f)
			.mapToPair(mapToPair_f)
			.reduceByKey(merge_f, hash_f, toStr_f, fromStr_f, 4)
			.collect();

	cout << "Result: " << endl;
	for (int i=0; i<wc.size(); i++) {
		cout << wc[i].v1 << ": " << wc[i].v2 << endl;
	}

	return 0;
}



