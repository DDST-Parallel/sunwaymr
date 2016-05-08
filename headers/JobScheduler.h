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
#include <pthread.h>
#include "Messaging.h"
#include "Scheduler.h"
#include "Task.h"
#include "TaskResult.h"
using std::string;
using std::vector;

#ifndef HOST_RESOURCE_DELIMITATION
#define HOST_RESOURCE_DELIMITATION " "
#endif

/*
 * A context will have only one JobScheduler.
 * JobScheduler will listen message,
 * and create new TaskSchedulers when context::runTasks() is invoked.
 */
class JobScheduler : public Messaging, public Scheduler {
public:
	JobScheduler();
	JobScheduler(string hostFilePath, string master, string appName, int listenPort);
	~JobScheduler();

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
	vector<Scheduler *> taskSchedulers;

	pthread_mutex_t mutex_job_scheduler;
	int nextJobID;
	vector<string> taskResultWorksOfNextJob;
	vector<string> taskResultsOfNextJob;

	void messageReceived(int localListenPort, string fromHost,
			int msgType, string &msg); // override Messaging
	void handleMessage(int localListenPort, string fromHost,
			int msgType, string &msg, int &retValue); // override Scheduler
};


#endif /* HEADERS_JOBSCHEDULER_H_ */
