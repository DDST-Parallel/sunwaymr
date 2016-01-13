/*
 * Pair.h
 *
 *  Created on: Jan 13, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_PAIR_H_
#define HEADERS_PAIR_H_

template <class K, class V>
class Pair {
public:
	Pair(K &k, V &v);
	~Pair();

	K v1;
	V v2;
};

#endif /* HEADERS_PAIR_H_ */
