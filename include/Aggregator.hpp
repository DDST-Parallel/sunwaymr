/*
 * Aggregator.hpp
 *
 *  Created on: 2016年2月23日
 *      Author: knshen
 */

#ifndef INCLUDE_AGGREGATOR_HPP_
#define INCLUDE_AGGREGATOR_HPP_

#include "Aggregator.h"

/*
 * constructor
 */
template <class V, class C>
Aggregator<V, C>::Aggregator(C (*cc)(V&), C (*mc)(C&, C&))
{
	createCombiner = cc;
	mergeCombiners = mc;
}

#endif /* INCLUDE_AGGREGATOR_HPP_ */
