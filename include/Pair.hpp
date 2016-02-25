/*
 * Pair.hpp
 *
 *  Created on: Jan 13, 2016
 *      Author: yupeng
 */

#ifndef PAIR_HPP_
#define PAIR_HPP_

#include "Pair.h"

#include <vector>
#include "Utils.hpp"
using std::vector;

template <class K, class V>
Pair<K, V>::Pair()
{

}

template <class K, class V>
Pair<K, V>::Pair(K k, V v)
: v1(k), v2(v)
{

}

template <class K, class V>
Pair<K, V>::~Pair()
{

}

template <class K, class V>
ostream& operator<< (ostream &out, Pair<K, V> &p) {
	out << p.v1 << PAIR_DELIMITATION << p.v2;
	return out;
}

template <class K, class V>
istream& operator>> (istream &in, Pair<K, V> &p) {
	string s;
	in >> s;
	vector<string> vs;
	splitString(s, vs, PAIR_DELIMITATION);
	stringstream s1(vs[0]);
	stringstream s2(vs[1]);
	K k;
	V v;
	s1 >> k;
	s2 >> v;
	p.v1 = k;
	p.v2 = v;
	return in;
}

#endif /* PAIR_HPP_ */


