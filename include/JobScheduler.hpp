
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

/*
 * default constructor
 */
JobScheduler::JobScheduler(){
	hostFilePath="";
	master="";
	appName="";
	listenPort=0;
	isMaster=0;
	selfIPIndex = -1;
	threadCountSum = 0;
	pthread_mutex_init(&mutex_job_scheduler, NULL); // initialization of mutex
	nextJobID = 0;
}

/*
 * constructor with host file path, master IP, application name and listen port
 */
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

	// read host resource file
	string fileContent;
	bool rd = readFile(hostFilePath, fileContent);
	if (!rd) {
		stringstream error;
		error << "JobScheduler: unable to read host resource file: " << hostFilePath;
		Logging::logError(error.str());
		exit(1);
	}

	threadCountSum = 0;
	pthread_mutex_init(&mutex_job_scheduler, NULL); // initialization of mutex
	nextJobID = 0;

	// read lines in file content, each line stands for a host with resource information
	stringstream fileContentStream(fileContent);
	string line;
	while(std::getline(fileContentStream,line,'\n')){
		vector<string> temp;
		splitString(line, temp, HOST_RESOURCE_DELIMITATION);
        if(temp.size() < 4) continue;
        IPVector.push_back(temp[0]);
        int tc = atoi(temp[1].c_str()); // available thread count
        threadCountVector.push_back(tc);
        threadCountSum += tc;
        memoryVector.push_back(atoi(temp[2].c_str())); // available memory
	}

	selfIPIndex = -1;
	for(unsigned int i=0;i<IPVector.size();i++){
		if (IPVector[i]==selfIP){
			selfIPIndex=i; // set the index of this host in IPVector
			break;
		}
	}

}

/*
 * destructor of JobScheduler.
 * to delete all created TaskSchedulers
 */
JobScheduler::~JobScheduler() {
	vector<Scheduler *>::iterator it;
	for(it = this->taskSchedulers.begin(); it != this->taskSchedulers.end(); ++it) {
		delete (*it);
	}
	this->taskSchedulers.clear();
}

/*
 * to hold listening port information of JobScheduler.
 */
struct xyz_job_scheduler_thread_data_ {
	JobScheduler *js;
	int port;
	xyz_job_scheduler_thread_data_(JobScheduler *js, int port) : js(js), port(port) { }
};

/*
 * thread function to start listening
 */
void *startSchedulerListening(void *data) {
	xyz_job_scheduler_thread_data_ *d = (xyz_job_scheduler_thread_data_ *)data;
	d->js->listenMessage(d->port); // will block at this line if succeeding with listening

	pthread_exit(NULL);
}

/*
 * to start JobScheduler, mainly to start listening
 */
bool JobScheduler::start(){
	pthread_t thread;
	struct xyz_job_scheduler_thread_data_ *data =
			new xyz_job_scheduler_thread_data_(this, listenPort);
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

/*
 * get the listening port of this JobScheduler
 */
int JobScheduler::getListenPort() {
	return this->listenPort;
}

/*
 * get the total number of threads to run tasks
 */
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

/*
 * get the IPs of all hosts
 */
vector<string> JobScheduler::getHosts() {
	return IPVector;
}

/*
 * to run tasks.
 * JobScheduler creates a new TaskScheduler for tasks of each Job.
 */
template <class T>
vector< TaskResult<T>* > JobScheduler::runTasks(vector<Task<T>*> &tasks){
	this->clearAllCache(); // clear cache of previous job

	pthread_mutex_lock(&mutex_job_scheduler);
	int jobID = nextJobID;
	nextJobID++;
	TaskScheduler<T> *ts = new TaskScheduler<T>(jobID,
			selfIP, selfIPIndex, master, appName,
			listenPort, IPVector, threadCountVector, memoryVector);
	taskSchedulers.push_back(ts);
	ts->preRunTasks(tasks); // pre-run tasks, for preparation

	// handle pre-arrived task results, some nodes may run faster than this node
	size_t n = taskResultWorksOfNextJob.size();
	if(n > 0) {
		for (unsigned int i=0; i<n; i++) {
			int ret = 0;
			ts->handleMessage(listenPort,
					taskResultWorksOfNextJob[i],
					A_TASK_RESULT,
					taskResultsOfNextJob[i],
					ret);
		}

		taskResultWorksOfNextJob.clear();
		taskResultsOfNextJob.clear();

		stringstream ss;
		ss << "JobScheduler::runTasks: [" << n << "] pre-arrived task results";
		Logging::logDebug(ss.str());
	}

	pthread_mutex_unlock(&mutex_job_scheduler);

	if (taskSchedulers.size() > 2) { // delete old task schedulers
		delete(taskSchedulers.front());
		taskSchedulers.erase(taskSchedulers.begin());
	}

	return ts->runTasks(tasks);
}

/*
 * messages are handled by JobSchduler firstly,
 * and may be delivered to TaskScheduler if valid
 */
void JobScheduler::messageReceived(int localListenPort, string fromHost,
		int msgType, string &msg) {
	if (localListenPort != this->listenPort || fromHost == "" || msg == "") return;

	if(Logging::getMask() <= 0) { // checkingg logging or not
		stringstream received;
		received << "JobScheduler: listening port: " << localListenPort
				<< ", message received from: " << fromHost
				<< ", message type: " << msgType
				<< ", message content: " << msg;
		Logging::logVerbose(received.str());
	}

	switch (msgType) {
		case A_TASK_RESULT:
		{
			if (taskSchedulers.size() > 0) {
				int ret = 0;
				taskSchedulers.back()->handleMessage(localListenPort, fromHost, msgType, msg, ret);

				if (ret > 0) { // task scheduler handle failed, job id mismatches
					pthread_mutex_lock(&mutex_job_scheduler);
					if (ret == this->nextJobID) {
						// a task result for next job. this occurs when master is slower than the slave
						// save the result
						taskResultWorksOfNextJob.push_back(fromHost);
						taskResultsOfNextJob.push_back(msg);
					}
					else if(ret == this->nextJobID - 1) { // try again
						taskSchedulers.back()->handleMessage(localListenPort, fromHost, msgType, msg, ret);
					}
					pthread_mutex_unlock(&mutex_job_scheduler);
				}
			}
			break;
		}
		case TASK_RESULT_LIST:
			if (taskSchedulers.size() > 0) {
				int ret = 0;
				taskSchedulers.back()->handleMessage(localListenPort, fromHost, msgType, msg, ret);
			}
			break;

		case RESULT_RENEED:
			// TODO parse message, select receiver in this->taskSchedulers

			break;

		case RESULT_RENEED_TOTAL:
			// TODO parse message, select receiver in this->taskSchedulers

			break;

		default:
			break;
	}
}

/*
 * override pure virtual function of Scheduler
 */
void JobScheduler::handleMessage(int localListenPort, string fromHost,
		int msgType, string &msg, int &retValue) {

}

#endif /* JOBSCHEDULER_HPP_ */
