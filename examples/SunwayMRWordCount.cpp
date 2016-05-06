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

/*
 * flat map text file block to words
 */
vector<string> flat_map_f1(TextFileBlock &t) {
	vector<string> ret;
	string data = t.blockData();
	stringstream ss(data);
	string word;
	while (ss >> word) {
		ret.push_back(word);
	}
	return ret;
}

/*
 * map each word to pair
 */
Pair<string, int> map_to_pair_f(string &s) {
	int i = 1;
	return Pair<string, int>(s, i);
}

/*
 * reduce function to sum counts of words
 */
Pair<string, int> reduce_by_key_f (Pair<string, int> &p1, Pair<string, int> &p2) {
	int i = p1.v2+p2.v2;
	return Pair<string, int>(p1.v1, i);
}

/*
 * main function
 */
int main(int argc, char *argv[]) {
	string start = currentDateTime(); // logging start time of computation
	cout << endl << "SunwayMR Word Count" << endl << endl;

	// set environment variables from console parameters
	SunwayMRContext sc("SunwayMRWordCount", argc, argv);
	vector<FileSource> fsv;
	FileSource fs = FileSource("192.168.1.165", "/opt/test-data/wc/1.txt");
	fsv.push_back(fs);
	vector< Pair<string, int> > wc = sc.textFile(fsv, FILE_SOURCE_FORMAT_LINE)
			->flatMap(flat_map_f1)
			->mapToPair(map_to_pair_f)
			->reduceByKey(reduce_by_key_f)
			->collect();

	cout << "Result: " << endl;
	for (unsigned int i=0; i<wc.size(); i++) {
		cout << "[" << i << "] " << wc[i].v1 << ": " << wc[i].v2 << endl;
	}

	Logging::logInfo(start);
	Logging::logInfo(currentDateTime());
	return 0;
}



