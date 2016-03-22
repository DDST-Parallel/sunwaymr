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
: valid(false) {

}

template <class K, class V>
Pair<K, V>::Pair(K k, V v)
: v1(k), v2(v), valid(true)
{

}

template <class K, class V>
Pair<K, V>::~Pair()
{

}

template <class K, class V>
bool Pair<K, V>::operator<(const Pair< K, V >& p) const {
	stringstream ss;
	ss << *this;
	string s(ss.str());
	ss.str(string());
	ss << p;
	return (s.compare(ss.str()) < 0);

}

template <class K, class V>
void Pair<K, V>::fromString(string &s, stringstream &ss) {
	vector<string> vs;
	vs = splitStringByDelimitationCouple(s, PAIR_DELIMITATION_LEFT, PAIR_DELIMITATION_RIGHT);
	if (vs.size() > 1) {
		K k;
		V v;
		ss.str(vs[0]);
		ss >> k;
		ss.str(vs[1]);
		ss.clear();
		ss >> v;
		this->v1 = k;
		this->v2 = v;

		this->valid = true;
	}
}

template <class K, class V>
ostream& operator<< (ostream &out, const Pair<K, V> &p) {
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
	std::stringstream ss;
	ss << in.rdbuf();
	string s(ss.str());
	vector<string> vs;
	vs = splitStringByDelimitationCouple(s, PAIR_DELIMITATION_LEFT, PAIR_DELIMITATION_RIGHT);
	if (vs.size() > 1) {
		K k;
		V v;
		ss.str(vs[0]);
		ss >> k;
		ss.str(vs[1]);
		ss.clear();
		ss >> v;
		p.v1 = k;
		p.v2 = v;

		p.valid = true;
	}
	return in;
}

#endif /* PAIR_HPP_ */


