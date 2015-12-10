/*
 * RDDTask.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef RDDTASK_H_
#define RDDTASK_H_

#include <vector>
#include <string>

#include "Partition.h"
#include "RDD.h"
#include "Task.h"
using std::vector;
using std::string;


template <class T, class U>
class RDDTask: public Task<U> {
public:
	RDDTask(RDD<T> &r, Partition &p);
	virtual ~RDDTask();
	virtual U& run() = 0;
	virtual string serialize(U &t) = 0;
	virtual U& deserialize(string s) = 0;
	virtual vector<string> preferredLocations();

	RDD<T> &rdd;
	Partition &partition;
};


#endif /* RDDTASK_H_ */
