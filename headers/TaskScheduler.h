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

int XYZ_TASK_SCHEDULER_RUN_TASK_MODE = 1; // 0: fork, 1: pthread

/*
 * TaskScheduler will run tasks of one job.
 * When running, each node will run one or more of given tasks.
 * Master will wait for every task result message.
 * Slaves will wait for task result list from Master.
 */
template <class T>
class TaskScheduler : public Messaging, public Scheduler {
public:
	TaskScheduler(int jobID, string selfIP, int selfIPIndex,
			string master, string appName, int listenPort,
			vector<string> &ip, vector<int> &threads, vector<int> &memory);
	~TaskScheduler();
	void preRunTasks(vector< Task<T>* > &tasks);
	vector< TaskResult<T>* > runTasks(vector< Task<T>* > &tasks);
	void finishTask(int task, T &value);

	void messageReceived(int localListenPort, string fromHost, int msgType, string &msg); // override Messaging
	void handleMessage(int localListenPort, string fromHost, int msgType, string &msg, int &retValue); // override Scheduler
	void increaseRunningThreadNum();
	void decreaseRunningThreadNum();
	bool getTaskResultString(int job, int task, string &result);
	bool getTaskResultListString(int job, string &result);

private:
	int jobID;
	string selfIP;
	int selfIPIndex;
	string master, appName;
	int listenPort;
	vector<string> IPVector;
	vector<int> threadCountVector, memoryVector, threadRemainVector;
	vector<string> taskOnIPVector;
	int isMaster;

	volatile int runningThreadNum;
	vector<pthread_t> startedThreads;
	vector<bool> resultReceived;
	volatile int receivedTaskResultNum;
	bool allTaskResultsReceived;
    bool taskResultListSent;
	vector< Task<T>* > tasks;
    vector< TaskResult<T>* > taskResults;

    pthread_mutex_t mutex_handle_message_ready;
    pthread_mutex_t mutex_all_tasks_received;
    pthread_mutex_t mutex_task_scheduler;
};


#endif /* HEADERS_TASKSCHEDULER_H_ */
