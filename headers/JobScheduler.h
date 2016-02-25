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
#include "Messaging.h"
#include "Scheduler.h"
#include "Task.h"
#include "TaskResult.h"
using std::string;
using std::vector;

#ifndef HOST_RESOURCE_DELIMITATION
#define HOST_RESOURCE_DELIMITATION " "
#endif

class JobScheduler : public Messaging {
public:
	JobScheduler();
	JobScheduler(string hostFilePath, string master, string appName, int listenPort);

	virtual bool start(); // may fail to start if listenPort is in use
	int getListenPort();
	virtual int totalThreads();
	vector<string> getHosts();

	template <class T> vector< TaskResult<T>* > runTasks(vector< Task<T>* > &tasks);

private:
	string hostFilePath, master, appName, selfIP;
	int listenPort, isMaster, selfIPIndex, threadCountSum;

	vector<string> IPVector;
	vector<int> threadCountVector;
	vector<int> memoryVector;
	vector<Scheduler*> taskSchedulers;

	void messageReceived(int localListenPort, string fromHost, int msgType, string msg);
};


#endif /* HEADERS_JOBSCHEDULER_H_ */
