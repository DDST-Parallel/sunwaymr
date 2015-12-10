/*
 * JobScheduler.h
 *
 *  Created on: Dec 10, 2015
 *      Author: yupeng
 */

#ifndef HEADERS_JOBSCHEDULER_H_
#define HEADERS_JOBSCHEDULER_H_

#include <string>
#include <vector>
#include "Scheduler.h"
#include "Task.h"
#include "TaskResult.h"
using std::string;
using std::vector;

class JobScheduler : public Scheduler {
public:
	JobScheduler();
	JobScheduler(string hostFilePath, string master, string appName, int listenPort);

	virtual bool start(); // may fail to start if listenPort is in use
	virtual int totalThreads();

	template <class T> vector< TaskResult<T>* > runTasks(vector< Task<T>* > &tasks);

private:
	string hostFilePath, master, appName;
	int listenPort, isMaster, selfIPIndex;

	vector<string> IPVector;
	vector<int> threadCountVector;
	vector<int> memoryVector;

	string getLocalIP();
	void messageReceived(int localListenPort, string fromHost, int msgType, string msg);
};


#endif /* HEADERS_JOBSCHEDULER_H_ */
