/*
 * Scheduler.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "Task.h"
#include "TaskResult.h"
#include "Logging.h"
#include "Message.h"
#include <vector>

using std::vector;

int vectorFind(vector<string>&  v, string value);
int vectorNonZero(vector<int>& v);

/*listening for new message from listenPort*/
void* listenReceive(void* data);

/*class Scheduler declaration*/
class Scheduler : public Logging {
public:
	Scheduler();
	Scheduler(string hostFilePath, string master, string appName, int listenPort); // master may be "local"
	virtual ~Scheduler();

	virtual bool start(); // may fail to start if listenPort is in use
	virtual int totalThreads();
	template <class T> vector< TaskResult<T>* > runTasks(vector< Task<T>* > &tasks);

	int isMaster;  //whether the node is master or not

	int socketfd;
	int client_fd;

	Message receiveMessage(int socketfd);
	bool sendMessage(Message message,int client_fd);

private:
	string hostFilePath, master, appName;
	int listenPort;

	int selfIPRank;

	vector<string> IPVector;
	vector<int> threadCountVector;
	vector<int> threadRemainVector;
	vector<int> memoryVector;

	//template <class T> void taskFinished(TaskResult<T> &t);
	bool myListen(int listenPort);
	string getLocalIP();

};


#endif /* SCHEDULER_H_ */
