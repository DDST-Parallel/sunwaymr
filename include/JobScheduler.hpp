
#ifndef JOBSCHEDULER_HPP_
#define JOBSCHEDULER_HPP_

#include "JobScheduler.h"

#include <fstream>
#include <iostream>
#include <pthread.h>

#include "Scheduler.hpp"
#include "TaskScheduler.hpp"
#include "Messaging.hpp"
#include "MessageType.hpp"
#include "Logging.hpp"
#include "Task.hpp"
#include "TaskResult.hpp"
#include "Utils.hpp"

using namespace std;

/* JobScheduler member functions */

JobScheduler::JobScheduler(){
	hostFilePath="";
	master="";
	appName="";
	listenPort=0;
	isMaster=0;
	selfIPIndex = -1;
	threadCountSum = 0;
}

JobScheduler::JobScheduler(string hostFP, string mas, string appN, int listenP){
	//initialize
	hostFilePath=hostFP;
	master=mas;
	appName=appN;
	listenPort=listenP;
	selfIP = getLocalHost();
	isMaster = 0;
	if (mas == "local" || mas == selfIP) {
		isMaster = 1;
	}

	//read resource file
	string fileContent;
	bool rd = readFile(hostFilePath, fileContent);
	if (!rd) {
		stringstream error;
		error << "JobScheduler: unable to read host resource file: " << hostFilePath;
		Logging::logError(error.str());
		exit(1);
	}

	threadCountSum = 0;
	stringstream fileContentStream(fileContent);
	string line;
	while(std::getline(fileContentStream,line,'\n')){
		vector<string> temp;
		splitString(line, temp, HOST_RESOURCE_DELIMITATION);
        if(temp.size() < 4) continue;
        IPVector.push_back(temp[0]);
        int tc = atoi(temp[1].c_str());
        threadCountVector.push_back(tc);
        threadCountSum += tc;
        memoryVector.push_back(atoi(temp[2].c_str()));
	}

	//vector <int> ::iterator s=find(IPVector.begin(),IPVector.end(),getLocalIP());
	selfIPIndex = -1;
	for(unsigned int i=0;i<IPVector.size();i++){
		if (IPVector[i]==selfIP){
			selfIPIndex=i;
			break;
		}
	}

}

struct JobSchedulerThreadData {
	JobScheduler &js;
	int port;
	JobSchedulerThreadData(JobScheduler &js, int port) : js(js), port(port) { }
};

void *startSchedulerListening(void *data) {
	JobSchedulerThreadData *d = (JobSchedulerThreadData *)data;
	d->js.listenMessage(d->port);

	pthread_exit(NULL);
}

bool JobScheduler::start(){
	pthread_t thread;
	struct JobSchedulerThreadData *data = new JobSchedulerThreadData(*this, listenPort);
	pthread_mutex_init(&mutex_listen_status, NULL);
	pthread_mutex_lock(&mutex_listen_status);
	int rc = pthread_create(&thread, NULL, startSchedulerListening, (void *)data);
	if (rc){
		Logging::logError("JobScheduler: failed to create thread to listen");
	}

	pthread_mutex_lock(&mutex_listen_status);
	pthread_mutex_unlock(&mutex_listen_status);

	if(getListenStatus() == SUCCESS) {
		return true;
	} else {
		return false;
	}
}

int JobScheduler::getListenPort() {
	return this->listenPort;
}

int JobScheduler::totalThreads(){

	int ret=0;

	if (master=="local"){
        //return thread num of local
		if(selfIPIndex >=0) {
			ret = threadCountVector[selfIPIndex];
		}
	}
	else{
        //return total thread of all node
		ret = threadCountSum;
	}
	return ret;
}

vector<string> JobScheduler::getHosts() {
	return IPVector;
}


template <class T>
vector< TaskResult<T>* > JobScheduler::runTasks(vector<Task<T>*> &tasks){
	int jobID = taskSchedulers.size();
	TaskScheduler<T> *ts = new TaskScheduler<T>(jobID, selfIP, selfIPIndex, master, appName,
			listenPort, IPVector, threadCountVector, memoryVector);
	taskSchedulers.push_back(ts);
	return ts->runTasks(tasks);
}

void JobScheduler::messageReceived(int localListenPort, string fromHost, int msgType, string msg) {
	if (localListenPort != this->listenPort || fromHost == "" || msg == "") return;

	stringstream received;
	received << "JobScheduler: listening port: " << localListenPort
			<< ", message received from: " << fromHost
			<< ", message type: " << msgType
			<< ", message content: " << msg;
	Logging::logDebug(received.str());

	switch (msgType) {
		case A_TASK_RESULT:
			if (taskSchedulers.size() > 0) {
				taskSchedulers[taskSchedulers.size()-1]->handleMessage(localListenPort, fromHost, msgType, msg);
			}
			break;

		case TASK_RESULT_LIST:
			if (taskSchedulers.size() > 0) {
				taskSchedulers[taskSchedulers.size()-1]->handleMessage(localListenPort, fromHost, msgType, msg);
			}
			break;

		case RESULT_RENEED:
			if (taskSchedulers.size() > 0) {
				taskSchedulers[taskSchedulers.size()-1]->handleMessage(localListenPort, fromHost, msgType, msg);
			}
			break;

		case RESULT_RENEED_TOTAL:
			if (taskSchedulers.size() > 0) {
				taskSchedulers[taskSchedulers.size()-1]->handleMessage(localListenPort, fromHost, msgType, msg);
			}
			break;

		default:
			break;
	}
}


#endif /* JOBSCHEDULER_HPP_ */
