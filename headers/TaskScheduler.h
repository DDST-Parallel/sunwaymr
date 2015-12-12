/*
 * TaskScheduler.h
 *
 *  Created on: Dec 10, 2015
 *      Author: yupeng
 */

#ifndef HEADERS_TASKSCHEDULER_H_
#define HEADERS_TASKSCHEDULER_H_

#include "Messaging.h"
#include "Task.h"
#include "TaskResult.h"

template <class T>
class TaskScheduler : public Messaging {
public:
	TaskScheduler(int jobID, string selfIP, int selfIPIndex, string master, string appName, int listenPort, vector<string> ip, vector<int> threads, vector<int> memory);
	vector< TaskResult<T>* > runTasks(vector< Task<T>* > &tasks);

	void messageReceived(int localListenPort, string fromHost, int msgType, string msg);

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
	vector<bool> resultReceived;
	int receivedTaskResultNum;
	bool allTaskResultsReceived;
	vector< Task<T>* > tasks;
    vector< TaskResult<T>* > taskResults;
};


#endif /* HEADERS_TASKSCHEDULER_H_ */
