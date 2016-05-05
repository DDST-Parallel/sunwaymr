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
#include "Utils.hpp"
#include "StringConvertion.hpp"

using namespace std;

template <class K, class V>
Pair<K, V>::Pair()
: valid(false) {

}

template <class K, class V>
Pair<K, V>::Pair(K &k, V &v)
: v1(k), v2(v), valid(true)
{

}

template <class K, class V>
Pair<K, V>::~Pair()
{

}

template <class K, class V>
bool Pair<K, V>::operator<(const Pair< K, V > &p) const {
	return (to_string(*this).compare(to_string(p)) < 0);

}

template <class K, class V>
bool Pair<K, V>::operator==(const Pair< K, V > &p) const
{
	return (v1 == p.v1 && v2 == p.v2);
}

#endif /* PAIR_HPP_ */


