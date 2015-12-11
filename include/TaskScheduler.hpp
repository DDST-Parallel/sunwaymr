#ifndef TASKSCHEDULER_HPP_
#define TASKSCHEDULER_HPP_

#include "TaskScheduler.h"

#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <sstream>
#include <stdlib.h>

#include "Scheduler.hpp"
#include "Messaging.hpp"
#include "MessageType.hpp"
#include "Logging.hpp"
#include "Utils.hpp"
#include "Task.hpp"
#include "TaskResult.hpp"

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
	allTaskResultsReceived = false;
	receivedTaskResultNum = 0;
}

template <class T>
struct thread_data {
	TaskScheduler<T> *taskScheduler;
	string master;
	int masterPort;
	int jobID;
	int taskID;
	Task<T> *task;
};

template <class T>
void *runTask(void *d) {
	struct thread_data<T> *data = (struct thread_data<T> *)d;
	TaskScheduler<T> *ts = data->taskScheduler;
	Task<T>* task = data->task;
	T& value = task->run();

	// send out task result
	stringstream ss;
	ss << data->jobID << " " << data->taskID << " " << task->serialize(value);
	ts->sendMessage(data->master, data->masterPort, A_TASK_RESULT, ss.str());

	pthread_exit(NULL);
}



template <class T>
vector< TaskResult<T>* > TaskScheduler<T>::runTasks(vector< Task<T>* > &tasks) {
	int taskNum=tasks.size();
	this->tasks = tasks;
	allTaskResultsReceived = false;
	receivedTaskResultNum = 0;
	taskResults = vector< TaskResult<T>* >(taskNum, NULL);

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
				index=vectorFind(IPVector,tasks[i]->preferredLocations()[i]);

				if(index!=-1 && threadRemainVector[index]>0){
					threadRemainVector[index]--;
					taskOnIPVector[i]=IPVector[index];
					needReDistributing[i]=0;
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
		}
	}


	// run tasks those been distributed to self
	int runOnThisNodeTaskNum = 0;
	int lanuchedTaskNum = 0;
	for (int i=0; i < taskNum; i++) {
		if (taskOnIPVector[i]==selfIP) runOnThisNodeTaskNum ++;
	}
	vector<int> launchedTask = vector<int>(taskNum);

	int threadsNum = 10; // 10 threads at most
	pthread_t threads[threadsNum];
	while (!allTaskResultsReceived) { // waiting until all results received
		if (lanuchedTaskNum == runOnThisNodeTaskNum) continue;

		for (int i=0; i < taskNum; i++) {
			if (taskOnIPVector[i]==selfIP && launchedTask[i]==0) { // pick non started task

				for (int j=0; j<threadsNum; j++) { // pick a free thread
					int kill_rc = pthread_kill(threads[j],0);
					if (threads[j]==0 || kill_rc == ESRCH || kill_rc == EINVAL) {
						thread_data<T> data = {
							this,
							master,
							listenPort,
							jobID,
							i,
							tasks[i]
						};
						int rc = pthread_create(&threads[j], NULL, runTask<T>, (void *)&data);
						if (rc){
							logError("TaskScheduler: failed to create thread to run Task.");
							exit(-1);
						}
						lanuchedTaskNum ++;

						break;
					}
				}
			}
		}
	}

	return taskResults;
}

template <class T>
void TaskScheduler<T>::messageReceived(int localListenPort, string fromHost, int msgType, string msg) {
	switch(msgType) {
	case A_TASK_RESULT:
	{
		if (isMaster == 1 && receivedTaskResultNum < tasks.size()) {
			// add task result to taskResults, if not duplicate
			vector<string> vs = splitString(msg, ' ');
			if (vs.size() == 3) {
				int jobID = atoi(vs[0].c_str());
				int taskID = atoi(vs[1].c_str());

				if (jobID == this->jobID
						&& taskID < tasks.size()
						&& taskResults[taskID] == NULL) {
					T& value = tasks[taskID]->deserialize(vs[2]);
					taskResults[taskID] = new TaskResult<T>(*tasks[taskID], value);
					receivedTaskResultNum ++;
				}
			}


			// check if all task results received
			if (receivedTaskResultNum == tasks.size()) {
				allTaskResultsReceived = true;

				// send out to other nodes
				stringstream resultList;
				for (unsigned int i=0; i<tasks.size(); i++) {
					T& value = taskResults[i]->value;
					resultList << jobID << " " << i << " " << tasks[i]->serialize(value);
					if (i != tasks.size()-1) resultList << ",";
				}
				string msg = resultList.str();
				for (unsigned int i=0; i<IPVector.size(); i++) {
					sendMessage(IPVector[i], listenPort, TASK_RESULT_LIST, msg);
				}

			}

		}

		break;
	}

	case TASK_RESULT_LIST:
	{
		if (isMaster == 0) {
			// save task results
			// initialize taskResults
			bool valid = true;
			vector<string> results = splitString(msg, ',');
			if (results.size() == tasks.size()) {
				for (unsigned int i=0; i<tasks.size(); i++) {
					vector<string> result = splitString(results[i], ' ');
					if (result.size() == 3) {
						int jobID = atoi(result[0].c_str());
						int taskID = atoi(result[1].c_str());

						if (jobID == this->jobID
								&& taskID < tasks.size()
								&& taskResults[taskID] == NULL) {
							T& value = tasks[taskID]->deserialize(result[2]);
							taskResults[taskID] = new TaskResult<T>(*tasks[taskID], value);
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

			if(valid) allTaskResultsReceived = true;
			else { // clean
				for (unsigned int i=0; i<tasks.size(); i++) {
					taskResults[i] = NULL;
				}
				receivedTaskResultNum = 0;
			}
		}

		break;
	}

	}
}


#endif /* TASKSCHEDULER_HPP_ */
