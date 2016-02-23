/*
 * Aggregator.h
 *
 *  Created on: 2016年2月23日
 *      Author: knshen
 */

#ifndef HEADERS_AGGREGATOR_H_
#define HEADERS_AGGREGATOR_H_

#include "Pair.h"

template <class K, class V, class C>

class Aggregator
{
public:
	Aggregator(C (*cc)(V), C (*mv)(Pair<C, V>), C (*mc)(Pair<C, C>));

	C (*createCombiner)(V);
	C (*mergeValue)(Pair<C, V>);
	C (*mergeCombiners)(Pair<C, C>);
};


#endif /* HEADERS_AGGREGATOR_H_ */
