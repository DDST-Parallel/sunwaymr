/*
 * Task.h
 *
 * Abstract super class of RDDTask.
 * Every action operator of RDD will create Tasks to run.
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef TASK_H_
#define TASK_H_

#include <vector>
#include <string>
using std::vector;
using std::string;

#ifndef REDUCE_TASK_DELIMITATION
#define REDUCE_TASK_DELIMITATION "\aRT\a"
#endif
#ifndef COLLECT_TASK_DELIMITATION
#define COLLECT_TASK_DELIMITATION "\aCT\a"
#endif
#ifndef TASK_RESULT_DELIMITATION
#define TASK_RESULT_DELIMITATION "\aTR\a"
#endif
#ifndef TASK_RESULT_LIST_DELIMITATION
#define TASK_RESULT_LIST_DELIMITATION "\aTRL\a"
#endif
#ifndef SHUFFLETASK_KV_DELIMITATION
#define SHUFFLETASK_KV_DELIMITATION "\aSKV\a"
#endif
#ifndef SHUFFLETASK_PARTITION_DELIMITATION
#define SHUFFLETASK_PARTITION_DELIMITATION "\aSP\a"
#endif
#ifndef SHUFFLETASK_EMPTY_DELIMITATION
#define SHUFFLETASK_EMPTY_DELIMITATION "\aSE\a"
#endif

long XYZ_CURRENT_TASK_ID = 1; // task id counter

template <class T>
class Task {
public:
	Task();
	virtual ~Task(){};
	virtual T run() = 0;
	virtual string serialize(T &t) = 0;
	virtual T deserialize(string &s) = 0;
	virtual vector<string> preferredLocations() { return vector<string>(0); }

	long taskID;
};


#endif /* TASK_H_ */
