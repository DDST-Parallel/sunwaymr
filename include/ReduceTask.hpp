/*
 * ReduceTask.hpp
 *
 *  Created on: Dec 4, 2015
 *      Author: yupeng
 */
#ifndef REDUCETASK_HPP_
#define REDUCETASK_HPP_

#include "ReduceTask.h"

#include <sstream>

#include "IteratorSeq.hpp"
#include "RDDTask.hpp"
#include "Utils.hpp"
using std::stringstream;

template <class T> ReduceTask<T>::ReduceTask(RDD<T> &r, Partition &p, T (*g)(T, T))
:RDDTask< T, vector<T> >::RDDTask(r, p), g(g)  {

}

template <class T> vector<T>&  ReduceTask<T>::run() {
	IteratorSeq<T> iter = RDDTask< T, vector<T> >::rdd.iteratorSeq(RDDTask< T, vector<T> >::partition);
	return iter.reduceLeft(g);
}

template <class T> string ReduceTask<T>::serialize(vector<T> &t) {
	stringstream ss;
	for (unsigned int i=0; i<t.size(); i++) {
		ss << t[i];
		if (i != t.size()-1) ss << REDUCE_TASK_DELIMITATION;
	}
	return ss.str();
}

template <class T> vector<T>& ReduceTask<T>::deserialize(string s) {
	vector<T> *elems = new vector<T>;
	vector<string> vs;
	splitString(s, vs, REDUCE_TASK_DELIMITATION);

	for(unsigned int i=0; i<vs.size(); i++) {
		std::stringstream ss(vs[i]);
		T t;
		ss >> t;
		elems->push_back(t);
	}
	return *elems;

//	std::stringstream ss(s);
//	std::string item;
//	while (std::getline(ss, item, ' ')) {
//		std::stringstream ss2(item);
//		T t;
//		ss2 >> t;
//		elems->push_back(t);
//	}
//	return *elems;
}


#endif /* REDUCETASK_HPP_ */
