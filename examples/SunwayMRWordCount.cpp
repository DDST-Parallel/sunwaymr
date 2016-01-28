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
using std::cout;
using std::endl;
using std::vector;
using std::stringstream;


long map_f(TextFileBlock t)
{
	string data = t.blockData();
	stringstream ss(data);
	string word;
	long wc = 0;
	while (ss >> word) wc++;
	return wc;
}

long reduce_f(long x, long y)
{
	return x + y;
}


int main(int argc, char *argv[]) {

	cout<< "SunwayMR Word Count Totally" << endl;

	SunwayMRContext sc("SunwayMRWordCountTotally", argc, argv);
	vector<FileSource> fsv;
	FileSource fs = FileSource("192.168.159.128", "text.dat");
	fsv.push_back(fs);
	long c = sc.textFile(fsv).map(map_f).reduce(reduce_f);
	cout << "Totally: " << c << endl;

	return 0;
}



