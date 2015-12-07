/*
 * RDDTask.cpp
 *
 *  Created on: Dec 4, 2015
 *      Author: yupeng
 */

#include "RDDTask.h"

template <class T, class U> RDDTask<T, U>::RDDTask(RDD<T> &r, Partition &p)
: rdd(r), partition(p) {
}

template <class T, class U> RDDTask<T, U>::~RDDTask () {

}

template <class T, class U> vector<string> RDDTask<T, U>::preferredLocations() {
	return vector<string>(0);
}


