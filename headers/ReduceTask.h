/*
 * ReduceTask.h
 *
 * RDD::reduce will create ReduceTasks, and run in context->JobScheduler->TaskScheduler
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef REDUCETASK_H_
#define REDUCETASK_H_

#include <vector>

#include "RDDTask.h"
using std::vector;


template <class T>
class ReduceTask : public RDDTask< T, vector<T> > {
public:
	ReduceTask(RDD<T> *r, Partition *p, T (*g)(T&, T&));
	vector<T> run();
	string serialize(vector<T> &t);
	vector<T> deserialize(string &s);

private:
	T (*g)(T&, T&);
};


#endif /* REDUCETASK_H_ */
