/*
 * TaskResult.h
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
	TaskResult();
	Task<T> task;
	T value;
};


#endif /* TASKRESULT_H_ */
