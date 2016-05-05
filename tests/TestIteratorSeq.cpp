/*
 * TestConfigFile.cpp
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include "IteratorSeq.hpp"
#include "RangeIteratorSeq.hpp"
#include "Utils.hpp"

using namespace std;

int print(int &x) {
	cout << x << endl;
	return x;
}

//stringstream ss;

int doubleInt(int &x) {
//	ss.str("");
//	ss.clear();
//	ss << x;
//	string s = ss.str();
//	ss.str("");
//	ss.clear();

//	string s = "";
//	s.clear();
//	char buffer[33];
//	snprintf(buffer, sizeof(buffer), "%d", x);
//	s = buffer;
//	s = "";
//	s.clear();

	return x * 2;
}

int sum(int &x, int &y) {
	return x + y;
}

int main() {

	cout << currentDateTime() << endl;
	cout << "hello" << endl;

	IteratorSeq<int> *seq = new RangeIteratorSeq<int>(1, 100000000, 1);

	//cout << seq->size() << endl;

	//seq->map<int>(print);

	IteratorSeq<int> *newSeq = seq->map(doubleInt);

	//newSeq->map<int>(print);

	vector<int> r = newSeq->reduceLeft(sum);

	if (r.size() > 0) cout << r[0] << endl;
	cout << currentDateTime() << endl;

	return 0;
}
