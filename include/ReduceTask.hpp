/*
 * ReduceTask.hpp
 *
 *  Created on: Dec 4, 2015
 *      Author: yupeng
 */
#ifndef REDUCETASK_HPP_
#define REDUCETASK_HPP_

#include "ReduceTask.h"

#include "IteratorSeq.hpp"
#include "RDDTask.hpp"
#include "Utils.hpp"
#include "StringConversion.hpp"

/*
 * constructor
 */
template <class T> ReduceTask<T>::ReduceTask(RDD<T> *r, Partition *p, T (*g)(T&, T&))
:RDDTask< T, vector<T> >::RDDTask(r, p), g(g)  {

}

/*
 * to run the reduce function on the data in the corresponding partition
 */
template <class T> vector<T> ReduceTask<T>::run() {
	IteratorSeq<T> *iter = RDDTask< T, vector<T> >::rdd->iteratorSeq(RDDTask< T, vector<T> >::partition);
	return iter->reduceLeft(g);
}

/*
 * to serialize the task result
 */
template <class T> string ReduceTask<T>::serialize(vector<T> &t) {
	string ret = "";
	for (unsigned int i=0; i<t.size(); i++) {
		ret += to_string(t[i]);
		if (i != t.size()-1) ret += REDUCE_TASK_DELIMITATION;
	}
	return ret;
}

/*
 * to deserialize a string to task result
 */
template <class T> vector<T> ReduceTask<T>::deserialize(string &s) {
	vector<T> elems;
	vector<string> vs;
	splitString(s, vs, REDUCE_TASK_DELIMITATION);

	for(unsigned int i=0; i<vs.size(); i++) {
		T t;
		from_string(t, vs[i]);
		elems.push_back(t);
	}
	return elems;

}


#endif /* REDUCETASK_HPP_ */
