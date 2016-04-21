/*
 * TaskResult.h
 *
 * Task result information of a Task.
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef TASKRESULT_H_
#define TASKRESULT_H_

#include "Task.h"


template <class T>
class TaskResult {
public:
	TaskResult(Task<T> *t, T &result);
	Task<T> *task;
	T value; // result value
};


#endif /* TASKRESULT_H_ */
