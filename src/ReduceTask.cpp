/*
 * ReduceTask.cpp
 *
 *  Created on: Dec 4, 2015
 *      Author: yupeng
 */

#include "ReduceTask.h"
#include "IteratorSeq.h"

template <class T> ReduceTask<T>::ReduceTask(RDD<T> &r, Partition &p, T (*g)(T, T))
:RDDTask< T, vector<T> >::RDDTask(r, p), g(g)  {

}

template <class T> vector<T>  ReduceTask<T>::run() {
	IteratorSeq<T> iter = RDDTask< T, vector<T> >::rdd.iteratorSeq(RDDTask< T, vector<T> >::partition);
	return iter.reduceLeft(g);
}


