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

#include <sstream>
#include <string>
using namespace std;


template <class T>
CollectTask<T>::CollectTask(RDD<T> &r, Partition &p)
:RDDTask< T, vector<T> >::RDDTask(r, p)
{
}

template <class T>
vector<T>& CollectTask<T>::run()
{
	IteratorSeq<T> iter = RDDTask< T, vector<T> >::rdd.iteratorSeq(RDDTask< T, vector<T> >::partition);
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
	vector<T> *ret = new vector<T>(iter.getVector());
	return *ret;
}

template <class T>
string CollectTask<T>::serialize(vector<T> &t)
{
	stringstream ss;
	for (unsigned int i=0; i<t.size(); i++) {
		ss << t[i];
		if (i != t.size()-1) ss << COLLECT_TASK_DELIMITATION;
	}
	return ss.str();
}

template <class T>
vector<T>& CollectTask<T>::deserialize(string s)
{
	vector<T> *elems = new vector<T>;
	vector<string> vs;
	splitString(s, vs, COLLECT_TASK_DELIMITATION);

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

#endif


