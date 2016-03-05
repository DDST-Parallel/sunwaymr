/*
 * Pair.h
 *
 * Duplicate class with pair class in C++.
 *
 *  Created on: Jan 13, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_PAIR_H_
#define HEADERS_PAIR_H_

#include <iostream>
using namespace std;

#ifndef PAIR_DELIMITATION_LEFT
#define PAIR_DELIMITATION_LEFT "\a(\a"
#endif
#ifndef PAIR_DELIMITATION_RIGHT
#define PAIR_DELIMITATION_RIGHT "\a)\a"
#endif

template <class K, class V>
class Pair {
public:
	Pair();
	Pair(K k, V v);
	~Pair();
	bool operator<(const Pair< K, V >& p) const;
	template <class T, class U>
	friend ostream& operator<< (ostream &out, const Pair<T, U> &p);
	template <class T, class U>
	friend istream& operator>> (istream &in, Pair<T, U> &p);

	K v1;
	V v2;
};

#endif /* HEADERS_PAIR_H_ */
