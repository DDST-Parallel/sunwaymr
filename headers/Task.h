/*
 * Task.h
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
#define REDUCE_TASK_DELIMITATION "\aREDUCE_TASK\a"
#endif
#ifndef COLLECT_TASK_DELIMITATION
#define COLLECT_TASK_DELIMITATION "\aCOLLECT_TASK\a"
#endif
#ifndef TASK_RESULT_DELIMITATION
#define TASK_RESULT_DELIMITATION "\aTASK_RESULT\a"
#endif
#ifndef TASK_RESULT_LIST_DELIMITATION
#define TASK_RESULT_LIST_DELIMITATION "\aTASK_RESULT_LIST\a"
#endif
#ifndef SHUFFLETASK_KV_DELIMITATION
#define SHUFFLETASK_KV_DELIMITATION "\aSHUFFLETASK_KV\a"
#endif
#ifndef SHUFFLETASK_PARTITION_DELIMITATION
#define SHUFFLETASK_PARTITION_DELIMITATION "\aSHUFFLETASK_PARTITION\a"
#endif
#ifndef SHUFFLETASK_EMPTY_DELIMITATION
#define SHUFFLETASK_EMPTY_DELIMITATION "\aSHUFFLETASK_EMPTY\a"
#endif

template <class T>
class Task {
public:
	Task();
	virtual ~Task(){};
	virtual T& run() = 0;
	virtual string serialize(T &t) = 0;
	virtual T& deserialize(string s) = 0;
	virtual vector<string> preferredLocations() { return vector<string>(0); }

	long taskID;
	static long current_task_id; // task id counter
};


#endif /* TASK_H_ */
