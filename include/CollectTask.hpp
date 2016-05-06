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
#include "StringConversion.hpp"

#include <string>
using namespace std;

/*
 * constructor
 */
template <class T>
CollectTask<T>::CollectTask(RDD<T> *r, Partition *p)
:RDDTask< T, vector<T> >::RDDTask(r, p)
{
}

/*
 * running the task.
 * for CollectTask, just return the partition data in vector.
 */
template <class T>
vector<T> CollectTask<T>::run()
{
	IteratorSeq<T> *iter = RDDTask< T, vector<T> >::rdd->iteratorSeq(RDDTask< T, vector<T> >::partition);
	return iter->getVector();
}

/*
 * serializing the task result.
 * the data set in vector are split by pre-defined delimitation.
 */
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

/*
 * deserializing task result from string.
 * firstly, split the string by delimitation,
 * then, convert the string to object of type T,
 * return all the objects in vector.
 */
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


