/*
 * SunwayMRPageRank.cpp
 *
 *  Created on: Feb 27, 2016
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

vector< Pair<string, string> > flat_map_f1(TextFileBlock t) {
	vector< Pair<string, string> > ret;
	string data = t.blockData(), line, word1, word2;
	stringstream ss1(data);
	while (std::getline(ss1, line)) {
		stringstream ss2(line);
		ss2 >> word1;
		ss2 >> word2;
		ret.push_back(Pair<string, string>(word1, word2));
	}
	return ret;
}

template <class K, class V>
Pair< K, V > map_to_pair_do_nothing_f(Pair< K, V > p) {
	return p;
}

Pair<string, double> map_values_f1(Pair<string, IteratorSeq<string> > p) {
	return Pair<string, double> (p.v1, 1.0);
}

vector< Pair<string, double> > flat_map_f2 (Pair<IteratorSeq<string>, double> p) {
	vector< Pair<string, double> > ret;
	double rank = p.v2;
	long size = p.v1.size();
	double contrib = rank / (double)size;
	for (long i=0; i<size; i++) {
		ret.push_back(Pair<string, double>(p.v1.at(i), contrib));
	}
	return ret;
}

Pair<string, double> reduce_by_key_f (Pair<string, double> p1, Pair<string, double> p2) {
	double d = p1.v2+p2.v2;
	return Pair<string, double>(p1.v1, d);
}

Pair<string, double> map_values_f2(Pair<string, double>  p) {
	return Pair<string, double> (p.v1, 0.15 + 0.85 * p.v2);
}

int main(int argc, char *argv[]) {

	cout << endl << "SunwayMR Page Rank" << endl << endl;

	SunwayMRContext sc("SunwayMRPageRank", argc, argv);
	vector<FileSource> fsv;
	FileSource fs = FileSource("192.168.1.66", "/tmp/2.txt");
	fsv.push_back(fs);
	int iteration = 10;

	PairRDD<string, IteratorSeq<string>, Pair<string, IteratorSeq<string> > >  &links =
			sc.textFile(fsv, FILE_SOURCE_FORMAT_LINE)
			.flatMap(flat_map_f1)
			.distinct()
			.mapToPair(map_to_pair_do_nothing_f<string, string>)
			.groupByKey()
			.mapToPair(map_to_pair_do_nothing_f<string, IteratorSeq<string> >);

	cout << "collecting................" << endl;
	vector<Pair<string,  IteratorSeq<string> > > ll = links.collect();
	cout << "links size: " << ll.size() << endl;
	for (unsigned int i=0; i<ll.size(); i++) {
		cout << ll[i].v1 << " has links: " << ll[i].v2.size() << endl;
	}

	PairRDD<string, double, Pair<string, double> > ranks =
			links.mapValues(map_values_f1)
			.mapToPair(map_to_pair_do_nothing_f<string, double>);

	vector<Pair<string, double> > origin = ranks.collect();
	cout << "ranks size: " << origin.size() << endl;
	for (unsigned int i=0; i<origin.size(); i++) {
		cout << origin[i].v1 << " has rank: " << origin[i].v2 << endl;
	}

	for (int i=0; i<iteration; i++) {
		ranks =
				links
				.join(ranks)
				.mapToPair(map_to_pair_do_nothing_f<string, Pair<IteratorSeq<string>, double> >)
				.values()
				.flatMap(flat_map_f2)
				.mapToPair(map_to_pair_do_nothing_f<string, double>)
				.reduceByKey(reduce_by_key_f)
				.mapToPair(map_to_pair_do_nothing_f<string, double>)
				.mapValues(map_values_f2);
	}

	vector<Pair<string, double> > output = ranks.collect();
	cout << "Result: " << endl;
	for (unsigned int i=0; i<output.size(); i++) {
		cout << output[i].v1 << " has rank: " << output[i].v2 << endl;
	}

	return 0;
}



