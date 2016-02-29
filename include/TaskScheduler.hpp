#ifndef TASKSCHEDULER_HPP_
#define TASKSCHEDULER_HPP_

#include "TaskScheduler.h"

#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <iostream>
#include <sstream>
#include <stdlib.h>

#include "Messaging.hpp"
#include "MessageType.hpp"
#include "Logging.hpp"
#include "Utils.hpp"
#include "Task.hpp"
#include "TaskResult.hpp"

using namespace std;

template <class T>
TaskScheduler<T>::TaskScheduler(int jobID, string selfIP,
		int selfIPIndex, string master, string appName,
		int listenPort, vector<string> ip,
		vector<int> threads, vector<int> memory)
: jobID(jobID), selfIP(selfIP), selfIPIndex(selfIPIndex), master(master), appName(appName),
  listenPort(listenPort), IPVector(ip), threadCountVector(threads), memoryVector(memory), isMaster(0) {
	if (master == "local" || master == selfIP) {
		isMaster = 1;
	}
	runningThreadNum = 0;
	allTaskResultsReceived = false;
	receivedTaskResultNum = 0;
}

template <class T>
struct thread_data {
	TaskScheduler<T> &taskScheduler;
	string master;
	int masterPort;
	int jobID;
	int taskID;
	Task<T> &task;

	thread_data(TaskScheduler<T> &t, string m, int mp, int ji, int ti, Task<T> &task)
	: taskScheduler(t), master(m), masterPort(mp), jobID(ji), taskID(ti), task(task) {}
};

template <class T>
void *runTask(void *d) {
	struct thread_data<T> *data = (struct thread_data<T> *)d;
	TaskScheduler<T> &ts = data->taskScheduler;
	Task<T> &task = data->task;
	T& value = task.run();

	// send out task result
	stringstream ss;
	ss << data->jobID << TASK_RESULT_DELIMITATION << data->taskID << TASK_RESULT_DELIMITATION << task.serialize(value);
	ts.sendMessage(data->master, data->masterPort, A_TASK_RESULT, ss.str());
	ts.decreaseRunningThreadNum();

	pthread_exit(NULL);
}



template <class T>
vector< TaskResult<T>* > TaskScheduler<T>::runTasks(vector< Task<T>* > &tasks) {

	stringstream runTasksInfo;
	runTasksInfo << "TaskScheduler: runTasks, job: [" << jobID << "], tasks size: [" << tasks.size() << "]";
	Logging::logInfo(runTasksInfo.str());

	int taskNum=tasks.size();
	this->tasks = tasks;

	runningThreadNum = 0;
	allTaskResultsReceived = false;
	receivedTaskResultNum = 0;
	resultReceived = vector<bool>(taskNum, false);

	taskOnIPVector = vector<string>(taskNum, selfIP);
	vector<int> needReDistributing(taskNum,1);

    threadRemainVector=vectorExpandNonNegativeSum(threadCountVector,taskNum);
    vectorFillNegative(threadRemainVector);


	//keep threadRemianVector and remainWaitNum locally
	for( int i=0;i<taskNum;i++){
		if(tasks[i]->preferredLocations().size()>0){
			int index=-1;

			int j;
            int plSize=tasks[i]->preferredLocations().size();
			for( j=0;j<plSize;j++){
				index=vectorFind(IPVector,tasks[i]->preferredLocations()[j]);

				if(index!=-1 && threadRemainVector[index]>0){
					threadRemainVector[index]--;
					taskOnIPVector[i]=IPVector[index];
					needReDistributing[i]=0;

					stringstream taskOnIP;
					taskOnIP << "TaskScheduler: task [" << i << "] of job[" << jobID << "] will run at " << taskOnIPVector[i];
					Logging::logDebug(taskOnIP.str());

					break;
				}
			}
//			if(j<plSize){
//				continue;
//			}
		}
	}

	//re-distribution:
	//1.preferred localtions do not meet with resources demand
	//2.no preferred locations

	for( int i=0;i<taskNum;i++){
		if(needReDistributing[i]==1){
			int index=vectorNonZero(threadRemainVector);
			threadRemainVector[index]--;
			taskOnIPVector[i]=IPVector[index];
			needReDistributing[i]=0;

			stringstream taskOnIP;
			taskOnIP << "TaskScheduler: task [" << i << "] of job[" << jobID << "] will run at " << taskOnIPVector[i];
			Logging::logDebug(taskOnIP.str());
		}
	}

	// run tasks those been distributed to self
	int runOnThisNodeTaskNum = 0;
	int lanuchedTaskNum = 0;
	for (int i=0; i < taskNum; i++) {
		if (taskOnIPVector[i]==selfIP) runOnThisNodeTaskNum ++;
	}
	vector<int> launchedTask = vector<int>(taskNum);

	pthread_mutex_init(&mutex_allTaskResultsReceived, NULL);
	pthread_mutex_lock(&mutex_allTaskResultsReceived);
	while (!allTaskResultsReceived) { // waiting until all results received
		if (lanuchedTaskNum == runOnThisNodeTaskNum) {
			pthread_mutex_lock(&mutex_allTaskResultsReceived);
			pthread_mutex_unlock(&mutex_allTaskResultsReceived);
			pthread_mutex_destroy(&mutex_allTaskResultsReceived);
			continue;
		}

		if (XYZ_TASK_SCHEDULER_RUN_TASK_MODE == 0) {
			// [0]run tasks by fork
			while (lanuchedTaskNum < runOnThisNodeTaskNum) {
				for (int i=0; i < taskNum; i++) {
					if (taskOnIPVector[i]==selfIP && launchedTask[i]==0) { // pick non started task
						if (fork()==0) { // !!! keep thread safety !!! no stdio !!!
							T& value = tasks[i]->run();
							stringstream ss;
							ss << jobID << TASK_RESULT_DELIMITATION << i << TASK_RESULT_DELIMITATION << tasks[i]->serialize(value);
							sendMessage(master, listenPort, A_TASK_RESULT, ss.str());
							exit(0);
						} else {
							lanuchedTaskNum ++;
						}
					}
				}
			}

		} else {
			// [1]run tasks by pthread
			int THREADS_NUM_MAX = 10; // 10 threads at most TODO configuration out of code
			while (runningThreadNum < THREADS_NUM_MAX
					&& lanuchedTaskNum < runOnThisNodeTaskNum) {
				for (int i=0; i < taskNum; i++) {
					if (taskOnIPVector[i]==selfIP && launchedTask[i]==0) { // pick non started task
						pthread_t thread;
						struct thread_data<T> *data = new thread_data<T> (*this, master,
							listenPort, jobID, i, *tasks[i]);
						int rc = pthread_create(&thread, NULL, runTask<T>, (void *)data);
						if (rc){
							Logging::logError("TaskScheduler: failed to create thread to run task");
							exit(-1);
						}
						startedThreads.push_back(thread);
						launchedTask[i] = 1;
						lanuchedTaskNum ++;
						increaseRunningThreadNum();
						break;
					}
				}
			}
		}

	}

	stringstream results;
	results << "TaskScheduler: \n" << "job[" << jobID << "] task results: \n";
	for (unsigned int i=0; i<taskResults.size(); i++) {
		results << "[" << i << "] " << taskResults[i]->task.serialize(taskResults[i]->value) << endl;
	}
	Logging::logDebug(results.str());

	return taskResults;
}

template <class T>
void TaskScheduler<T>::messageReceived(int localListenPort, string fromHost, int msgType, string msg) {

}

template <class T>
void TaskScheduler<T>::handleMessage(int localListenPort, string fromHost, int msgType, string msg) {
	switch(msgType) {
	case A_TASK_RESULT:
	{
		if (isMaster == 1 && receivedTaskResultNum < tasks.size()) {

			// add task result to taskResults, if not duplicate
			vector<string> vs;
			splitString(msg, vs, TASK_RESULT_DELIMITATION);
			if (vs.size() >= 2) {
				int jobID = atoi(vs[0].c_str());
				int taskID = atoi(vs[1].c_str());

				if (jobID == this->jobID
						&& taskID < tasks.size()
						&& !resultReceived[taskID]) {
					if (vs.size() >=3) {
						T& value = tasks[taskID]->deserialize(vs[2]);
						taskResults.push_back(new TaskResult<T>(*tasks[taskID], value));
					} else {
						T& value = tasks[taskID]->deserialize("");
						taskResults.push_back(new TaskResult<T>(*tasks[taskID], value));
					}
					resultReceived[taskID] = true;
					receivedTaskResultNum ++;

					stringstream receivedDebug;
					receivedDebug << "TaskScheduler: master: a task result of job["
							<< jobID << "] received, totally "
							<< receivedTaskResultNum << " results of " << tasks.size()
							<< " tasks received";
					Logging::logDebug(receivedDebug.str());
				}
			}


			// check if all task results received
			if (receivedTaskResultNum == tasks.size()) {
				Logging::logInfo("TaskScheduler: master: sending out results...");

				// send out to other nodes
				stringstream resultList;
				for (unsigned int i=0; i<tasks.size(); i++) {
					T& value = taskResults[i]->value;
					resultList << jobID << TASK_RESULT_DELIMITATION << i << TASK_RESULT_DELIMITATION << tasks[i]->serialize(value);
					if (i != tasks.size()-1) resultList << TASK_RESULT_LIST_DELIMITATION;
				}
				string msg = resultList.str();
				for (unsigned int i=0; i<IPVector.size(); i++) {
					sendMessage(IPVector[i], listenPort, TASK_RESULT_LIST, msg);
				}

				Logging::logInfo("TaskScheduler: results sent");

				allTaskResultsReceived = true;
				pthread_mutex_unlock(&mutex_allTaskResultsReceived);
			}

		}

		break;
	}

	case TASK_RESULT_LIST:
	{
		if (isMaster == 0) {

			Logging::logInfo("TaskScheduler: task result list received");

			// save task results
			// initialize taskResults
			bool valid = true;
			vector<string> results;
			splitString(msg, results, TASK_RESULT_LIST_DELIMITATION);
			if (results.size() == tasks.size()) {
				for (unsigned int i=0; i<tasks.size(); i++) {
					vector<string> vs;
					splitString(results[i], vs, TASK_RESULT_DELIMITATION);
					if (vs.size() >= 2) {
						int jobID = atoi(vs[0].c_str());
						int taskID = atoi(vs[1].c_str());

						if (jobID == this->jobID
								&& taskID < tasks.size()
								&& !resultReceived[taskID]) {
							if (vs.size() >=3) {
								T& value = tasks[taskID]->deserialize(vs[2]);
								taskResults.push_back(new TaskResult<T>(*tasks[taskID], value));
							} else {
								T& value = tasks[taskID]->deserialize("");
								taskResults.push_back(new TaskResult<T>(*tasks[taskID], value));
							}
							resultReceived[taskID] = true;
							receivedTaskResultNum ++;
						} else {
							valid = false;
							break;
						}
					} else {
						valid = false;
						break;
					}
				}
			} else {
				valid = false;
			}

			if(valid) {
				allTaskResultsReceived = true;
				pthread_mutex_unlock(&mutex_allTaskResultsReceived);
			}
			else { // clean

				Logging::logWarning("TaskScheduler: result list invalid, cleaning...");

				for (unsigned int i=0; i<tasks.size(); i++) {
					taskResults.clear();
				}
				receivedTaskResultNum = 0;
			}
		} else {
			// master had all results
		}

		break;
	}

	}
}

template <class T>
void TaskScheduler<T>::increaseRunningThreadNum() {
	runningThreadNum++;
}

template <class T>
void TaskScheduler<T>::decreaseRunningThreadNum() {
	runningThreadNum--;
}


#endif /* TASKSCHEDULER_HPP_ */

