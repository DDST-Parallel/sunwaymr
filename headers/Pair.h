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

#ifndef PAIR_DELIMITATION_LEFT
#define PAIR_DELIMITATION_LEFT "\a(\a"
#endif
#ifndef PAIR_DELIMITATION_RIGHT
#define PAIR_DELIMITATION_RIGHT "\a)\a"
#endif

/*
 * Duplicate class with pair class in C++.
 */
template <class K, class V>
class Pair {
public:
	Pair();
	Pair(K &k, V &v);
	~Pair();
	bool operator<(const Pair< K, V > &p) const;
	bool operator==(const Pair< K, V > &p) const;

	K v1;
	V v2;

	bool valid;
};

#endif /* HEADERS_PAIR_H_ */
