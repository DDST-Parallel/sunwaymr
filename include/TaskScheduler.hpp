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

template<class T>
TaskScheduler<T>::TaskScheduler(int jobID, string selfIP, int selfIPIndex,
		string master, string appName, int listenPort, vector<string> ip,
		vector<int> threads, vector<int> memory) :
		jobID(jobID), selfIP(selfIP), selfIPIndex(selfIPIndex), master(master), appName(
				appName), listenPort(listenPort), IPVector(ip), threadCountVector(
				threads), memoryVector(memory), isMaster(0) {
	if (master == "local" || master == selfIP) {
		isMaster = 1;
	}
	lastJobId = jobID - 1;
	runningThreadNum = 0;
	allTaskResultsReceived = false;
	receivedTaskResultNum = 0;
}

template<class T>
struct thread_data {
	TaskScheduler<T> &taskScheduler;
	string master;
	int masterPort;
	int jobID;
	int taskID;
	Task<T> &task;

	thread_data(TaskScheduler<T> &t, string m, int mp, int ji, int ti,
			Task<T> &task) :
			taskScheduler(t), master(m), masterPort(mp), jobID(ji), taskID(ti), task(
					task) {
	}
};

template<class T>
void *runTask(void *d) {
	struct thread_data<T> *data = (struct thread_data<T> *) d;
	TaskScheduler<T> &ts = data->taskScheduler;
	Task<T> &task = data->task;
	T& value = task.run();

	// send out task result
	stringstream ss;
	ss << data->jobID << TASK_RESULT_DELIMITATION << data->taskID
			<< TASK_RESULT_DELIMITATION <<ts.selfIP<<TASK_RESULT_DELIMITATION<< task.serialize(value);

	//record temp result in case the second time to sendMessage
	ts.currJobResultPart[data->taskID]=ss.str();

	int isFirst=1;
	ss<<TASK_RESULT_DELIMITATION<<isFirst;
	ts.sendMessage(data->master, data->masterPort, A_TASK_RESULT, ss.str());

	//record that task has been done
	ts.resultDone[data->taskID]=true;
	ts.decreaseRunningThreadNum();

	pthread_exit(NULL);
}

template<class T>
vector<TaskResult<T>*> TaskScheduler<T>::runTasks(vector<Task<T>*> &tasks) {

	stringstream runTasksInfo;
	runTasksInfo << "TaskScheduler: runTasks, job: [" << jobID
			<< "], tasks size: [" << tasks.size() << "]";
	Logging::logInfo(runTasksInfo.str());

	int taskNum = tasks.size();
	this->tasks = tasks;

	runningThreadNum = 0;
	allTaskResultsReceived = false;
	receivedTaskResultNum = 0;
	resultReceived = vector<bool>(taskNum, false);

	taskOnIPVector = vector<string>(taskNum, selfIP);
	vector<int> needReDistributing(taskNum, 1);

	threadRemainVector = vectorExpandNonNegativeSum(threadCountVector, taskNum);
	vectorFillNegative(threadRemainVector);

	resultDone = vector<bool>(taskNum, false);
	lastJobResultPart = vector<string>(taskNum, "");
	currJobResultPart = vector<string>(taskNum, "");
	lastJobResultTotal = "";

	//check every node valid or not
	//suppose that node(slave) can send message to master itself -> when master receive result ->check node valid
	IPVectorValid = vector<int>(taskNum, 1);

	//keep threadRemianVector and remainWaitNum locally
	for (int i = 0; i < taskNum; i++) {
		if (tasks[i]->preferredLocations().size() > 0) {
			int index = -1;

			int j;
			int plSize = tasks[i]->preferredLocations().size();
			for (j = 0; j < plSize; j++) {
				index = vectorFind(IPVector, tasks[i]->preferredLocations()[j]);

				if (index != -1 && threadRemainVector[index] > 0) {
					threadRemainVector[index]--;
					taskOnIPVector[i] = IPVector[index];
					needReDistributing[i] = 0;

					stringstream taskOnIP;
					taskOnIP << "TaskScheduler: task [" << i << "] of job["
							<< jobID << "] will run at " << taskOnIPVector[i];
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
	//1.preferred locations do not meet with resources demand
	//2.no preferred locations

	for (int i = 0; i < taskNum; i++) {
		if (needReDistributing[i] == 1) {
			int index = vectorNonZero(threadRemainVector);
			if (index != -1) {
				threadRemainVector[index]--;
				taskOnIPVector[i] = IPVector[index];
				needReDistributing[i] = 0;

				stringstream taskOnIP;
				taskOnIP << "TaskScheduler: task [" << i << "] of job[" << jobID
						<< "] will run at " << taskOnIPVector[i];
				Logging::logDebug(taskOnIP.str());
			} else {
				//no thread left when heavy load
				//threadRemainVector is big enough to deal all tasks
			}
		}
	}

	//count the task num which will run on every node(salve or master)
	if (isMaster == 1) {
		IPVectorValid = vectorMatchCount(IPVector, taskOnIPVector);
	}

	// run tasks those been distributed to self
	int runOnThisNodeTaskNum = 0;
	int lanuchedTaskNum = 0;
	for (int i = 0; i < taskNum; i++) {
		if (taskOnIPVector[i] == selfIP)
			runOnThisNodeTaskNum++;
	}
	vector<int> launchedTask = vector<int>(taskNum);

	//time counter
	clock_t t_start = clock();
	clock_t t_end = clock();
	clock_t period = t_end - t_start;

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
				for (int i = 0; i < taskNum; i++) {
					if (taskOnIPVector[i] == selfIP && launchedTask[i] == 0) { // pick non started task
						if (fork() == 0) { // !!! keep thread safety !!! no stdio !!!
							T& value = tasks[i]->run();
							stringstream ss;
							int isFirst = 1;
							ss << jobID << TASK_RESULT_DELIMITATION << i
									<< TASK_RESULT_DELIMITATION
									<<taskOnIPVector[i]
									<<TASK_RESULT_DELIMITATION
									<< tasks[i]->serialize(value);

							//record temp result in case the second time to sendMessage
							currJobResultPart[i] = ss.str();

							ss << TASK_RESULT_DELIMITATION << isFirst;
							sendMessage(master, listenPort, A_TASK_RESULT,
									ss.str());

							//record that task has been done
							resultDone[i] = true;
							exit(0);
						} else {
							lanuchedTaskNum++;
						}
					}
				}
			}

		} else {
			// [1]run tasks by pthread
			int THREADS_NUM_MAX = 10; // 10 threads at most TODO configuration out of code
			while (runningThreadNum < THREADS_NUM_MAX
					&& lanuchedTaskNum < runOnThisNodeTaskNum) {
				for (int i = 0; i < taskNum; i++) {
					if (taskOnIPVector[i] == selfIP && launchedTask[i] == 0) { // pick non started task
						pthread_t thread;
						struct thread_data<T> *data = new thread_data<T>(*this,
								master, listenPort, jobID, i, *tasks[i]);
						int rc = pthread_create(&thread, NULL, runTask<T>,
								(void *) data);
						if (rc) {
							Logging::logError(
									"TaskScheduler: failed to create thread to run task");
							exit(-1);
						}
						startedThreads.push_back(thread);
						launchedTask[i] = 1;
						lanuchedTaskNum++;
						increaseRunningThreadNum();
						break;
					}
				}
			}
		}

		t_end = clock();
		period = t_end - t_start;

		if (isMaster == 1 && period > 10000 * taskNum) {
			//master only collect part of task result, ask slaves for the rest
			/*still need concrete strategy in face with node down -> IPVectorValid*/
			for (int i = 0; i < taskNum; i++) {
				if (resultReceived[i] == false) {
					int isAccordingOrigin = 1;
					stringstream resultReneed;
					resultReneed << jobID << TASK_RESULT_DELIMITATION << i<< TASK_RESULT_DELIMITATION << isAccordingOrigin;
					sendMessage(taskOnIPVector[i], listenPort, RESULT_RENEED,resultReneed.str());

					//in case taskOnIPVector[i] fails -> re-schedule <- random keep  master no wait in vain
					/*some case the taskOnIPVector[i], taskOnIPVector[index] both down-> wait next while loop to pick another ip*/
					isAccordingOrigin = 0;
					stringstream resultReneed2;
					resultReneed2 << jobID << TASK_RESULT_DELIMITATION << i<< TASK_RESULT_DELIMITATION << isAccordingOrigin;
					int index = vectorNonZeroExcept(threadRemainVector,IPVector, taskOnIPVector[i]);
					sendMessage(IPVector[index], listenPort, RESULT_RENEED,resultReneed2.str());
				}
			}

			stringstream receivedDebug;
			receivedDebug<< "TaskScheduler: master: part of task results of job"<< jobID << " still lack, ask slave.";
			Logging::logInfo(receivedDebug.str());
		}

		if (isMaster == 0 && period > 10000 * taskNum) {
			//slave re-ask job results from master
			stringstream jobResultReneedTotal;
			int taskMarkTotal = -1;
			jobResultReneedTotal << jobID << TASK_RESULT_LIST_DELIMITATION<< taskMarkTotal << TASK_RESULT_LIST_DELIMITATION << selfIP;
			sendMessage(master, listenPort, RESULT_RENEED_TOTAL,jobResultReneedTotal.str());

			stringstream receivedDebug;
			receivedDebug << "TaskScheduler: slave: job" << jobID<< " result fails to obtain, ask master again";
			Logging::logInfo(receivedDebug.str());
		}
	}

	stringstream results;
	results << "TaskScheduler: \n" << "job[" << jobID << "] task results: \n";
	for (unsigned int i = 0; i < taskResults.size(); i++) {
		results << "[" << i << "] "
				<< taskResults[i]->task.serialize(taskResults[i]->value)
				<< endl;
	}
	Logging::logDebug(results.str());

	//update job and task results backup
	lastJobResultPart = currJobResultPart;
	currJobResultPart.clear();

	return taskResults;
}

template<class T>
void TaskScheduler<T>::messageReceived(int localListenPort, string fromHost,
		int msgType, string msg) {

}

template<class T>
void TaskScheduler<T>::handleMessage(int localListenPort, string fromHost,
		int msgType, string msg) {
	switch (msgType) {
	case A_TASK_RESULT: {
		if (isMaster == 1 && receivedTaskResultNum < tasks.size()) {

			// add task result to taskResults, if not duplicate
			vector<string> vs;
			splitString(msg, vs, TASK_RESULT_DELIMITATION);
			if (vs.size() >= 3) {
				int jobID = atoi(vs[0].c_str());
				int taskID = atoi(vs[1].c_str());
				string slaveIp=vs[2].c_str();

				if (jobID == this->jobID && taskID < tasks.size()
						&& !resultReceived[taskID]) {
					if (vs.size() >= 5) {
						T& value = tasks[taskID]->deserialize(vs[3]);
						taskResults.push_back(
								new TaskResult<T>(*tasks[taskID], value));

						int isFirst=atoi(vs[4].c_str());
						if(isFirst==1){
							int index=vectorFind(IPVector,slaveIp);
							IPVectorValid[index]--;
						}else{
							//second time to ask slave for result->do nothing
						}
					} else {
						T& value = tasks[taskID]->deserialize("");
						taskResults.push_back(
								new TaskResult<T>(*tasks[taskID], value));

						int isFirst=atoi(vs[3].c_str());
						if(isFirst==1){
							int index=vectorFind(IPVector,slaveIp);
							IPVectorValid[index]--;
						}else{
							//second time to ask slave for result->do nothing
						}

					}
					resultReceived[taskID] = true;
					receivedTaskResultNum++;

					stringstream receivedDebug;
					receivedDebug
							<< "TaskScheduler: master: a task result of job["
							<< jobID << "] received, totally "
							<< receivedTaskResultNum << " results of "
							<< tasks.size() << " tasks received";
					Logging::logDebug(receivedDebug.str());
				}else {
					//jobID=this->jobID+1, do nothing, then wait to ask slave for this part
					//refer to this part in runTask() above
				}
			}

			// check if all task results received
			if (receivedTaskResultNum == tasks.size()) {
				Logging::logInfo(
						"TaskScheduler: master: sending out results...");

				// send out to other nodes
				stringstream resultList;
				for (unsigned int i = 0; i < tasks.size(); i++) {
					T& value = taskResults[i]->value;
					resultList << jobID << TASK_RESULT_DELIMITATION << i
							<< TASK_RESULT_DELIMITATION
							<< tasks[i]->serialize(value);
					if (i != tasks.size() - 1)
						resultList << TASK_RESULT_LIST_DELIMITATION;
				}
				string msg = resultList.str();
				for (unsigned int i = 0; i < IPVector.size(); i++) {
					sendMessage(IPVector[i], listenPort, TASK_RESULT_LIST, msg);
				}

				Logging::logInfo("TaskScheduler: results sent");

				//check which node is invalid
				vector<int> invalidIPIndex=vectorNonZeroAllIndex(IPVectorValid);
				if(invalidIPIndex[0]!=-1){
					stringstream nodeDebug;
					nodeDebug<< "TaskScheduler: master: discover invalid node of high possibility-----";
					//int ipIndex=-1;
					for (unsigned int i = 0; i < invalidIPIndex.size(); ++i)
					{
						//inform all the other node of which node is invalid
						//delete these node in resource file->?-> modify in other file


						//record these debug information
						int ipIndex=invalidIPIndex[i];
						nodeDebug<<" Node IP: "<<IPVector[ipIndex]<<";";

					}
					nodeDebug<<"when run job :"<<jobID;
					Logging::logDebug(nodeDebug.str());
				}

				allTaskResultsReceived = true;
				pthread_mutex_unlock(&mutex_allTaskResultsReceived);

				//update and backup for the last job results
				this->lastJobResultTotal = msg;
			}
		}

		break;
	}

	case TASK_RESULT_LIST: {
		if (isMaster == 0) {

			Logging::logInfo("TaskScheduler: task result list received");

			// save task results
			// initialize taskResults
			bool valid = true;
			vector<string> results;
			splitString(msg, results, TASK_RESULT_LIST_DELIMITATION);
			if (results.size() == tasks.size()) {
				for (unsigned int i = 0; i < tasks.size(); i++) {
					vector<string> vs;
					splitString(results[i], vs, TASK_RESULT_DELIMITATION);
					if (vs.size() >= 2) {
						int jobID = atoi(vs[0].c_str());
						int taskID = atoi(vs[1].c_str());

						if (jobID == this->jobID && taskID < tasks.size()
								&& !resultReceived[taskID]) {
							if (vs.size() >= 3) {
								T& value = tasks[taskID]->deserialize(vs[2]);
								taskResults.push_back(
										new TaskResult<T>(*tasks[taskID],
												value));
							} else {
								T& value = tasks[taskID]->deserialize("");
								taskResults.push_back(
										new TaskResult<T>(*tasks[taskID],
												value));
							}
							resultReceived[taskID] = true;
							receivedTaskResultNum++;
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

			if (valid) {
				allTaskResultsReceived = true;
				pthread_mutex_unlock(&mutex_allTaskResultsReceived);

				//update and backup for the last job results
				this->lastJobResultTotal = msg;
			} else { // clean

				Logging::logWarning(
						"TaskScheduler: result list invalid, cleaning...");

				for (unsigned int i = 0; i < tasks.size(); i++) {
					taskResults.clear();
				}
				receivedTaskResultNum = 0;
			}
		} else {
			// master had all results
		}

		break;
	}
	case RESULT_RENEED: {
		if (isMaster == 0) {

			// record taskids, then send the corresponding results again.
			vector<string> vs;
			vector<int> resultNeedIds;
			splitString(msg, vs, TASK_RESULT_DELIMITATION);
			if (vs.size() >= 3) {
				int jobID = atoi(vs[0].c_str());
				int taskID = atoi(vs[1].c_str());
				int isAccordingOrigin=atoi(vs[2].c_str());

				int isFirst=0;
				if(isAccordingOrigin==1){
					if (jobID == this->lastJobId) {
						stringstream ss;
						ss<<lastJobResultPart[taskID]<<TASK_RESULT_DELIMITATION<<isFirst;
						sendMessage(master, listenPort, A_TASK_RESULT,ss.str());

						stringstream sendDebug;
						sendDebug<< "TaskScheduler: slave: the task ["<<taskID<< "] result of job["
								<< jobID << "] on " << selfIP
								<< " has been sent to master again.";
						Logging::logDebug(sendDebug.str());
					}
					if (jobID == this->jobID) {
						if (resultDone[taskID] == true) {
							stringstream ss;
							ss<<currJobResultPart[taskID]<<TASK_RESULT_DELIMITATION<<isFirst;
							sendMessage(master, listenPort, A_TASK_RESULT,ss.str());

							stringstream sendDebug;
							sendDebug<< "TaskScheduler: slave: the task ["<<taskID<< "] result of job["
									<< jobID << "] on " << selfIP
									<< " has been sent to master again.";
							Logging::logDebug(sendDebug.str());
						} else {
						   //still pending, will run later -> do nothing
						}
					}
				}

				//re-schedule divide: maybe one task of last job, or one task of this job
				if(isAccordingOrigin==0){
					//run this task
					T& value = this->tasks[taskID]->run();
					stringstream ss;
					ss << jobID << TASK_RESULT_DELIMITATION << taskID
							<< TASK_RESULT_DELIMITATION
							<<selfIP
							<<TASK_RESULT_DELIMITATION
							<< tasks[taskID]->serialize(value)
							<<TASK_RESULT_DELIMITATION
							<<isFirst;
					sendMessage(master, listenPort, A_TASK_RESULT,ss.str());

					//update all task result marks
					if(jobID==this->lastJobId){
						this->lastJobResultPart[taskID]=ss.str();
					}
					if(jobID==this->jobID){
						resultDone[taskID]=true;
						taskOnIPVector[taskID]=selfIP;
						this->currJobResultPart[taskID]=ss.str();
					}

					stringstream sendDebug;
					sendDebug<< "TaskScheduler: slave: the re-scheduling task ["<<taskID<< "] result of job["
							<< jobID << "] on " << selfIP
							<< " has been sent to master.";
					Logging::logDebug(sendDebug.str());
				}
			}
		}
		break;
	}

	case RESULT_RENEED_TOTAL: {
		if (isMaster == 1) {
			vector<string> vs;
			splitString(msg, vs, TASK_RESULT_DELIMITATION);
			if (vs.size() >= 3) {
				int jobID = atoi(vs[0].c_str());
				int taskID = atoi(vs[1].c_str());
				string slaveIp = vs[2].c_str();

				if (taskID == -1 && jobID == this->lastJobId) {
					string msg = lastJobResultTotal;
					sendMessage(slaveIp, listenPort, TASK_RESULT_LIST, msg);
				}
				if (taskID == -1 && jobID == this->jobID) {
					//master send out job result and enter the next job
					//no current job result, only part tasks results
				}
			}
		}
		break;
	}

	}
}

template<class T>
void TaskScheduler<T>::increaseRunningThreadNum() {
	runningThreadNum++;
}

template<class T>
void TaskScheduler<T>::decreaseRunningThreadNum() {
	runningThreadNum--;
}

#endif /* TASKSCHEDULER_HPP_ */

