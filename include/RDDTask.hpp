/*
 * RDDTask.hpp
 *
 *  Created on: Dec 4, 2015
 *      Author: yupeng
 */

#ifndef RDDTASK_HPP_
#define RDDTASK_HPP_


#include "RDDTask.h"

#include "Partition.hpp"
#include "RDD.hpp"
#include "Task.hpp"

template <class T, class U> RDDTask<T, U>::RDDTask(RDD<T> &r, Partition &p)
: rdd(r), partition(p) {
}

template <class T, class U> RDDTask<T, U>::~RDDTask () {

}

template <class T, class U> vector<string> RDDTask<T, U>::preferredLocations() {
	return vector<string>(0);
}


#endif /* RDDTASK_HPP_ */
