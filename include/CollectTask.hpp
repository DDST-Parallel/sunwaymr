/*
 * CollectTask.hpp
 *
 *  Created on: 2016年1月17日
 *      Author: knshen
 */

#ifndef COLLECTTASK_HPP_
#define COLLECTTASK_HPP_

#include "CollectTask.h"
#include "IteratorSeq.hpp"
#include "RDDTask.hpp"
#include "StringConvertion.hpp"

#include <string>
using namespace std;


template <class T>
CollectTask<T>::CollectTask(RDD<T> *r, Partition *p)
:RDDTask< T, vector<T> >::RDDTask(r, p)
{
}

template <class T>
vector<T> CollectTask<T>::run()
{
	IteratorSeq<T> *iter = RDDTask< T, vector<T> >::rdd->iteratorSeq(RDDTask< T, vector<T> >::partition);
//	if(iter.type == 0)
//	{
//		vector<T> *tmp = new vector<T>;
//		T start = iter.getStart();
//		T end = iter.getEnd();
//		T step = iter.getStep();
//		T i = start;
//		for(; i<end; i+=step)
//			tmp->push_back(i);
//
//		if(i == end && iter.isInclusive() == 1)
//			tmp->push_back(end);
//
//		return *tmp;
//	}
//	// type = 1
	return iter->getVector();
}

template <class T>
string CollectTask<T>::serialize(vector<T> &t)
{
	string ret = "";
	for (unsigned int i=0; i<t.size(); i++) {
		ret += to_string(t[i]);
		if (i != t.size()-1) ret += COLLECT_TASK_DELIMITATION;
	}
	return ret;
}

template <class T>
vector<T> CollectTask<T>::deserialize(string &s)
{
	vector<T> elems;
	vector<string> vs;
	splitString(s, vs, COLLECT_TASK_DELIMITATION);

	for(unsigned int i=0; i<vs.size(); i++) {
		T t;
		from_string(t, vs[i]);
		elems.push_back(t);
	}
	return elems;
}

#endif


