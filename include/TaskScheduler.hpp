#ifndef TASKSCHEDULER_HPP_
#define TASKSCHEDULER_HPP_

#include "TaskScheduler.h"

#include <pthread.h>
#include <sstream>

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
}

template <class T>
vector< TaskResult<T>* > TaskScheduler<T>::runTasks(vector< Task<T>* > &tasks) {
	allTaskResultsReceived = false;

	int taskNum=tasks.size();
	taskOnIPVector = vector<string>(taskNum); // TODO
	vector<int> needReDistributing(taskNum,1);

    threadRemainVector=vectorExpandNonNegativeSum(threadCountVector,taskNum);
    vectorFillNegative(threadRemainVector);

	int taskLauched=0;
	vector< TaskResult<T>* > resultsArray;
	vector< Task<T>* > taskDeal;

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

    //create thread pool with maximum- thread
	ThreadPool <T> tp(taskNum);

	//add to thread pool
    for (int i=0;i<taskNum;i++){
        //preferredIPVector refer to self -> add task to thread pool
        if( taskOnIPVector[i]==selfIP){
			tp.addToThreadPool(*tasks[i], i);
			taskDeal.push_back(tasks[i]);
			taskLauched++;

		}
        //preferredIPVector refer to others -> do nothing
	    //else{}

	}
    vector< TaskResult<T>* > taskResults
    return waitForTaskResults<T>(taskResults);


	return resultsArray;
}

template <class T>
vector< TaskResult<T>* > TaskScheduler<T>::waitForTaskResults(vector< TaskResult<T>* > &taskResults) {

	while (!allTaskResultsReceived) {

	}

	return taskResults;

}

template <class T>
void TaskScheduler<T>::messageReceived(int localListenPort, string fromHost, int msgType, string msg) {
	switch(msgType) {
	case A_TASK_RESULT:
	{
		if (isMaster == 1) {
			// TODO add task result to taskResults, if not duplicate


			// TODO check if all task results received...


			allTaskResultsReceived = true;

			// TODO send out to other nodes

		}

		break;
	}

	case TASK_RESULT_LIST:
	{
		if (isMaster == 0) {
			// TODO save task results
			// initialize taskResults

			allTaskResultsReceived = true;

		}

		break;
	}

	}
}


#endif /* TASKSCHEDULER_HPP_ */

