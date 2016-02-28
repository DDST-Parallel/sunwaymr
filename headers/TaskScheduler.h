/*
 * TaskScheduler.h
 *
 *  Created on: Dec 10, 2015
 *      Author: yupeng
 */

#ifndef HEADERS_TASKSCHEDULER_H_
#define HEADERS_TASKSCHEDULER_H_

#include <pthread.h>

#include "Messaging.h"
#include "Scheduler.h"
#include "Task.h"
#include "TaskResult.h"

int XYZ_TASK_SCHEDULER_RUN_TASK_MODE = 0; // 0: fork, 1: pthread

template <class T>
class TaskScheduler : public Messaging, public Scheduler {
public:
	TaskScheduler(int jobID, string selfIP, int selfIPIndex, string master, string appName, int listenPort, vector<string> ip, vector<int> threads, vector<int> memory);
	vector< TaskResult<T>* > runTasks(vector< Task<T>* > &tasks);

	void messageReceived(int localListenPort, string fromHost, int msgType, string msg); // override Messaging
	void handleMessage(int localListenPort, string fromHost, int msgType, string msg); // override Scheduler
	void increaseRunningThreadNum();
	void decreaseRunningThreadNum();

private:
	int jobID;
	string selfIP;
	int selfIPIndex;
	string master, appName;
	int listenPort;
	vector<string> IPVector;
	vector<int> threadCountVector, memoryVector, threadRemainVector;
	vector<string>  taskOnIPVector;
	int isMaster;

	int runningThreadNum;
	vector<pthread_t> startedThreads;
	vector<bool> resultReceived;
	int receivedTaskResultNum;
	bool allTaskResultsReceived;
	vector< Task<T>* > tasks;
    vector< TaskResult<T>* > taskResults;

    pthread_mutex_t mutex_allTaskResultsReceived;
};


#endif /* HEADERS_TASKSCHEDULER_H_ */
