
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
}

void JobScheduler::init(string hostFP, string mas, string appN, int listenP){
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
	ifstream ins;
	const char * hfp=hostFilePath.c_str();
	ins.open(hfp,ios::in);

	if(!ins){
		logError("JobScheduler: fail to open host file");
	}
	string line;

	while(!ins.eof()){
        getline(ins,line);
        vector<string>temp;
        splitString(line,temp," ");
        if(temp.size() < 3) continue;
        IPVector.push_back(temp[0]);
        threadCountVector.push_back(atoi(temp[1].c_str()));
        memoryVector.push_back(atoi(temp[2].c_str()));
	}
	ins.close();

	//vector <int> ::iterator s=find(IPVector.begin(),IPVector.end(),getLocalIP());
	selfIPIndex = -1;
	for(unsigned int i=0;i<IPVector.size();i++){
		if (IPVector[i]==selfIP){
			selfIPIndex=i;
			break;
		}
	}


}


void *startListening(void *data) {
	JobScheduler * js = (JobScheduler *)data;
	js->listenMessage(js->getListenPort());

	pthread_exit(NULL);
}

bool JobScheduler::start(){
	pthread_t thread;
	int rc = pthread_create(&thread, NULL, startListening, (void *)this);
	if (rc){
		logError("JobScheduler: failed to create thread to listen");
	}
	while(getListenStatus() == NA);

	if(getListenStatus() == FAILURE) {
		return false;
	} else {
		return true;
	}
}

int JobScheduler::getListenPort() {
	return this->listenPort;
}

int JobScheduler::totalThreads(){

	int totalThreadNum=0;

	if (master=="local"){
        //return thread num of local
		if(selfIPIndex >=0) {
			totalThreadNum = threadCountVector[selfIPIndex];
		}

	}
	else{
        //return total thread of all node
		int tc=threadCountVector.size();
		for (int i=0; i<tc;i++){
			totalThreadNum+=threadCountVector[i];
		}
	}
	return totalThreadNum;
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

	switch (msgType) {
		case A_TASK_RESULT:
			if (taskSchedulers.size() > 0) {
				taskSchedulers[taskSchedulers.size()-1]->messageReceived(localListenPort, fromHost, msgType, msg);
			}
			break;

		case TASK_RESULT_LIST:
			if (taskSchedulers.size() > 0) {
				taskSchedulers[taskSchedulers.size()-1]->messageReceived(localListenPort, fromHost, msgType, msg);
			}
			break;

		default:
			break;
	}

}


#endif /* JOBSCHEDULER_HPP_ */
