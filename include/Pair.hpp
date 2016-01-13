/*
 * Pair.hpp
 *
 *  Created on: Jan 13, 2016
 *      Author: yupeng
 */

#ifndef PAIR_HPP_
#define PAIR_HPP_

#include "Pair.h"

template <class K, class V>
Pair<K, V>::Pair(K &k, V &v)
: v1(k), v2(v)
{

}

template <class K, class V>
Pair<K, V>::~Pair()
{

}

#endif /* PAIR_HPP_ */


