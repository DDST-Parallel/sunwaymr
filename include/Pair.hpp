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
#include <string>
#include <sstream>
#include "Utils.hpp"
using std::vector;
using std::string;
using std::stringstream;

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
	out << PAIR_DELIMITATION_LEFT
			<< p.v1
			<< PAIR_DELIMITATION_RIGHT
			<< PAIR_DELIMITATION_LEFT
			<< p.v2
			<< PAIR_DELIMITATION_RIGHT;
	return out;
}

template <class K, class V>
istream& operator>> (istream &in, Pair<K, V> &p) {
	std::stringstream buffer;
	buffer << in.rdbuf();
	string s(buffer.str());
	vector<string> vs;
	vs = splitStringByDelimitationCouple(s, PAIR_DELIMITATION_LEFT, PAIR_DELIMITATION_RIGHT);
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


