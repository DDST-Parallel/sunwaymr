/*
 * ThreadPool.h
 *
 *  Created on: Dec 7, 2015
 *      Author: shi qiuwei
 */

#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_

#include <pthread.h>
#include <vector>
#include <string>

#include "Logging.h"
#include "Task.h"

using std::vector;


//thread pool
template <class T>
class ThreadPool:public Logging{
public:
	ThreadPool(int threadNum=10);
	int addToThreadPool(Task<T> &task,int tid);
	int stopAll();
	int getCurrentOccupyThreadNum();
    vector <int> taskId;
    vector <T> taskValue;

	vector < Task<T>* > tasksList;

    static bool shutdown;
    int taskLauched;
    pthread_t *pthread_id;

    static pthread_mutex_t m_pthreadMutex;   //thread sync lock
    static pthread_cond_t m_pthreadCond;     //thread sync condition variable
protected:
	static void* threadFunc(void * threadData=NULL); //new thread call func
	int moveToIdle(pthread_t tid);       //thread finished, then return to idle thread queue
	int moveToBusy(pthread_t tid);       //move to busy thread
	int create();                               //create thread pool
private:

};


#endif /* THREAD_POOL_H_ */
