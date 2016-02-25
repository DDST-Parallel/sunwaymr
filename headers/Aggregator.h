/*
 * Aggregator.h
 *
 *  Created on: 2016年2月23日
 *      Author: knshen
 */

#ifndef HEADERS_AGGREGATOR_H_
#define HEADERS_AGGREGATOR_H_

template <class V, class C>
class Aggregator
{
public:
	Aggregator(C (*cc)(V), C (*mc)(C, C));

	C (*createCombiner)(V);
	C (*mergeCombiners)(C, C);
};


#endif /* HEADERS_AGGREGATOR_H_ */
