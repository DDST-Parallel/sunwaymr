/*
 * Scheduler.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <vector>
#include "Task.h"
#include "TaskResult.h"
#include "Logging.h"
#include "Message.h"
using std::vector;

void* listenReceive(void* data);
class Scheduler : public Logging {
public:
	Scheduler();
	Scheduler(string hostFilePath, string master, string appName, int listenPort); // master may be "local"
	virtual ~Scheduler();

	virtual bool start(); // may fail to start if listenPort is in use
	virtual int totalThreads();
	template <class T> vector< TaskResult<T>* > runTasks(vector< Task<T>* > &tasks);

	int isMaster;

	int socketfd;
	Message receiveMessage(int socketfd);
private:
	string hostFilePath, master, appName;
	int listenPort;

	int client_fd;

    int finishedCount;
	vector<string> IPVector;
	vector<int> threadCountVector;
	vector<int> memoryVector;

	//template <class T> void taskFinished(TaskResult<T> &t);
	bool myListen(int listenPort);
	string getLocalIP();
	//void* listenReceive(void * data);
	bool sendMessage(Message message,int client_fd);


	// other new functions below...
};


#endif /* SCHEDULER_H_ */
