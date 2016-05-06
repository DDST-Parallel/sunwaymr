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
#include "StringConversion.hpp"

using namespace std;

/*
 * default constructor
 */
template <class K, class V>
Pair<K, V>::Pair()
: valid(false) {

}

/*
 * constructor
 */
template <class K, class V>
Pair<K, V>::Pair(K &k, V &v)
: v1(k), v2(v), valid(true)
{

}

/*
 * destructor
 */
template <class K, class V>
Pair<K, V>::~Pair()
{

}

/*
 * for STL map when Pair<K, V> is the key-type of map
 */
template <class K, class V>
bool Pair<K, V>::operator<(const Pair< K, V > &p) const {
	return (to_string(*this).compare(to_string(p)) < 0);

}

/*
 * for unordered_map when Pair<K, V> is the key-type of unordered_map
 */
template <class K, class V>
bool Pair<K, V>::operator==(const Pair< K, V > &p) const
{
	return (v1 == p.v1 && v2 == p.v2);
}

#endif /* PAIR_HPP_ */


