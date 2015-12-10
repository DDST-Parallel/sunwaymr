/*
 * TestConfigFile.cpp
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#include <iostream>
#include "IteratorSeq.hpp"

using namespace std;

int print(int x) {
	cout << x << endl;
	return x;
}

int doubleInt(int x) {
	return x * 2;
}

int sum(int x, int y) {
	return x + y;
}

int main() {

	cout << "hello" << endl;

	IteratorSeq<int> seq = IteratorSeq<int>(1, 10, 1);

	cout << seq.size() << endl;

	seq.map<int>(print);

	IteratorSeq<int> newSeq = seq.map<int>(doubleInt);

	newSeq.map<int>(print);

	vector<int> r = newSeq.reduceLeft(sum);

	if (r.size() > 0) cout << r[0] << endl;

	return 0;
}
