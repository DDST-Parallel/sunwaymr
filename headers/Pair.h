/*
 * Pair.h
 *
 *  Created on: Jan 13, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_PAIR_H_
#define HEADERS_PAIR_H_

#include <iostream>
using namespace std;

#ifndef PAIR_DELIMITATION
#define PAIR_DELIMITATION "\aPAIR\a"
#endif

template <class K, class V>
class Pair {
public:
	Pair();
	Pair(K k, V v);
	~Pair();
	template <class T, class U>
	friend ostream& operator<< (ostream &out, Pair<T, U> &p);
	template <class T, class U>
	friend istream& operator>> (istream &in, Pair<T, U> &p);

	K v1;
	V v2;
};

#endif /* HEADERS_PAIR_H_ */
