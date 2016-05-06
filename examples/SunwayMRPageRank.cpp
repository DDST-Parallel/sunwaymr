/*
 * SunwayMRPageRank.cpp
 *
 * Page rank example of SunwayMR.
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

/*
 * flat map text file block to pairs of link
 */
vector< Pair<string, string> > flat_map_f1(TextFileBlock &t) {
	vector< Pair<string, string> > ret;
	string data = t.blockData(), line;
	char buffer1[128], buffer2[128];
	stringstream ss1(data);
	while (std::getline(ss1, line)) {
		if (line.empty()) continue;
		sscanf(line.c_str(), "%s %s", buffer1, buffer2);
		string word1 = buffer1, word2 = buffer2;
		ret.push_back(Pair<string, string>(word1, word2));
	}
	return ret;
}

/*
 * map to pair without change original data set.
 */
template <class K, class V>
Pair< K, V > map_to_pair_do_nothing_f(Pair< K, V > &p) {
	return p;
}

/*
 * for each start of link, set rank as 1.0.
 * this is the initialization of all ranks.
 */
Pair<string, double> map_values_f1(Pair<string, VectorIteratorSeq<string> > &p) {
	double r = 1.0;
	return Pair<string, double> (p.v1, r);
}

/*
 * flat map function used after join.
 * divide ranks of each link.
 */
vector< Pair<string, double> > flat_map_f2 (Pair<VectorIteratorSeq<string>, double> &p) {
	vector< Pair<string, double> > ret;
	double rank = p.v2;
	long size = p.v1.size();
	double contrib = rank / (double)size;
	for (long i=0; i<size; i++) {
		string left = p.v1.at(i);
		ret.push_back(Pair<string, double>(left, contrib));
	}
	return ret;
}

/*
 * reduce function.
 * sum all ranks of same link side.
 */
Pair<string, double> reduce_by_key_f (Pair<string, double> &p1, Pair<string, double> &p2) {
	double d = p1.v2+p2.v2;
	return Pair<string, double>(p1.v1, d);
}

/*
 * scale
 */
Pair<string, double> map_values_f2(Pair<string, double> &p) {
	double right = 0.15 + 0.85 * p.v2;
	return Pair<string, double> (p.v1, right);
}

/*
 * main function
 */
int main(int argc, char *argv[]) {
	string start = currentDateTime(); // logging start time of page rank
	cout << endl << "SunwayMR Page Rank" << endl << endl;

	SunwayMRContext sc("SunwayMRPageRank", argc, argv);
	vector<FileSource> fsv;
	FileSource fs = FileSource("192.168.1.165", "/opt/test-data/pr/1.txt");
	fsv.push_back(fs);
	int iteration = 10; //set iteration

	PairRDD<string, VectorIteratorSeq<string>, Pair<string, VectorIteratorSeq<string> > >  *links =
			sc.textFile(fsv, FILE_SOURCE_FORMAT_LINE)
			->flatMap(flat_map_f1)
			->distinct()
			->mapToPair(map_to_pair_do_nothing_f<string, string>)
			->groupByKey();
	links->setSticky(true); // set this RDD as sticky, will not be deleted automatically

	PairRDD<string, double, Pair<string, double > > *ranks =
			links->mapValues(map_values_f1)
					->mapToPair(map_to_pair_do_nothing_f<string, double>);

	for (int i=0; i<iteration; i++) { // iterations of page ranking
		ranks =
				links
				->join(ranks)
				->values()
				->flatMap(flat_map_f2)
				->mapToPair(map_to_pair_do_nothing_f<string, double>)
				->reduceByKey(reduce_by_key_f)
				->mapValues(map_values_f2);

	}
	vector<Pair<string, double> > output = ranks->collect();

	cout << "Result: " << endl;
	for (unsigned int i=0; i<output.size(); i++) {
		cout << "[" << i << "] " << output[i].v1 << " has rank: " << output[i].v2 << endl;
	}

	Logging::logInfo(start);
	Logging::logInfo(currentDateTime());
	return 0;
}



