/*
 * Aggregator.h
 *
 *  Created on: 2016年2月23日
 *      Author: knshen
 */

#ifndef HEADERS_AGGREGATOR_H_
#define HEADERS_AGGREGATOR_H_

/*
 * To keep two function that will be used in ShuffledTask::run, ShuffledRDD::iteratorSeq
 */
template <class V, class C>
class Aggregator
{
public:
	Aggregator(C (*cc)(V&), C (*mc)(C&, C&)); // constructor

	C (*createCombiner)(V&); // a function to create combiners
	C (*mergeCombiners)(C&, C&); // a function to merge combiners
};


#endif /* HEADERS_AGGREGATOR_H_ */
