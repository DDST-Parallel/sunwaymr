/*
 * TaskScheduler.h
 *
 *  Created on: Dec 10, 2015
 *      Author: yupeng
 */

#ifndef HEADERS_TASKSCHEDULER_H_
#define HEADERS_TASKSCHEDULER_H_

#include "Scheduler.h"
#include "Task.h"
#include "TaskResult.h"

template <class T>
class TaskScheduler : public Scheduler {
public:
	TaskScheduler(int jobID, string selfIP, int selfIPIndex, string master, string appName, int listenPort, vector<string> ip, vector<int> threads, vector<int> memory);
	vector< TaskResult<T>* > runTasks(vector< Task<T>* > &tasks);

private:
	int jobID;
	string selfIP;
	int selfIPIndex;
	string master, appName;
	int listenPort;
	vector<string> IPVector;
	vector<int> threadsVector, memoryVector, threadRemainVector;
	vector<string>  preferredIPVector;
	int isMaster;
	bool allTaskResultsReceived;

	vector< TaskResult<T>* > waitForTaskResults(vector< TaskResult<T>* > &taskResults);
	void messageReceived(int localListenPort, string fromHost, int msgType, string msg);
};


#endif /* HEADERS_TASKSCHEDULER_H_ */
