/*
 * Aggregator.hpp
 *
 *  Created on: 2016年2月23日
 *      Author: knshen
 */

#ifndef INCLUDE_AGGREGATOR_HPP_
#define INCLUDE_AGGREGATOR_HPP_

#include "Aggregator.h"

template <class V, class C>
Aggregator<V, C>::Aggregator(C (*cc)(V), C (*mv)(Pair<C, V>), C (*mc)(Pair<C, C>))
{
	createCombiner = cc;
	mergeValue = mv;
	mergeCombiners = mc;
}


#endif /* INCLUDE_AGGREGATOR_HPP_ */
