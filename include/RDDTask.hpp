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

/*
 * constructor
 */
template <class T, class U> RDDTask<T, U>::RDDTask(RDD<T> *r, Partition *p)
: rdd(r), partition(p) {
}

/*
 * destructor
 */
template <class T, class U> RDDTask<T, U>::~RDDTask () {

}

/*
 * to get preferred locations of the partition in this task.
 */
template <class T, class U> vector<string> RDDTask<T, U>::preferredLocations() {
	return rdd->preferredLocations(partition);
}


#endif /* RDDTASK_HPP_ */
