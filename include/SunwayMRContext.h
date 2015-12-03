/*
 * SunwayMRContext.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef SUNWAYMRCONTEXT_H_
#define SUNWAYMRCONTEXT_H_

#include <string>
#include "ParallelArray.h"
#include "Task.h"
#include "TaskResult.h"
#include "Logging.h"
using std::string;

class SunwayMRContext : public Logging {
public:
	template <class T> ParallelArray<T> parallelize(T t1, T t2);
	template <class T> ParallelArray<T> parallelize(T t1, T t2, int numSlices);
	template <class T> vector<TaskResult<T>> runTasks(vector<Task<T>> &tasks);

private:
	Scheduler scheduler;

	bool init(string hostsFilePath);
};


#endif /* SUNWAYMRCONTEXT_H_ */
