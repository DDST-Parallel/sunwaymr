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

vector<string> flat_map_f1(TextFileBlock t) {
	vector<string> ret;
	string data = t.blockData();
	stringstream ss(data);
	string word;
	while (ss >> word) {
		ret.push_back(word);
	}
	return ret;
}

Pair<string, int> mapToPair_f(string s) {
	int i = 1;
	return Pair<string, int>(s, i);
}

Pair<string, int> reduceByKey_f (Pair<string, int> p1, Pair<string, int> p2) {
	int i = p1.v2+p2.v2;
	return Pair<string, int>(p1.v1, i);
}

int main(int argc, char *argv[]) {

	cout << endl << "SunwayMR Word Count" << endl << endl;

	SunwayMRContext sc("SunwayMRWordCount", argc, argv);
	vector<FileSource> fsv;
	FileSource fs = FileSource("192.168.99.13", "/tmp/1.txt");
	fsv.push_back(fs);
	vector< Pair<string, int> > wc = sc.textFile(fsv, FILE_SOURCE_FORMAT_LINE)
			.flatMap(flat_map_f1)
			.mapToPair(mapToPair_f)
			.reduceByKey(reduceByKey_f)
			.collect();

	cout << "Result: " << endl;
	for (unsigned int i=0; i<wc.size(); i++) {
		cout << wc[i].v1 << ": " << wc[i].v2 << endl;
	}

	return 0;
}



