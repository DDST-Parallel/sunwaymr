/*
 * ThreadPool.hpp
 *
 *  Created on: Dec 7, 2015
 *      Author: shi qiuwei
 */
#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_


#include "ThreadPool.h"

#include <pthread.h>
#include <vector>
#include <sstream>
#include <stdlib.h>

#include "Task.hpp"
#include "Logging.hpp"

using std::vector;
using std::stringstream;

template <class T>
ThreadPool<T>::ThreadPool(int threadNum){
	this->taskLauched=threadNum;
	logInfo("ThreadPool: create thread number: "+threadNum);
	create();
}

template <class T>
void* ThreadPool<T>::threadFunc(void * threadData){

	ThreadPool<T>* tp = (ThreadPool<T> *) threadData;

	pthread_t tid=pthread_self();
	while(1){
		pthread_mutex_lock(&m_pthreadMutex);
		while(tp->tasksList.size()==0 && ! shutdown){
			pthread_cond_wait(&m_pthreadCond,&m_pthreadMutex);
		}

		if(shutdown){
			pthread_mutex_unlock(&m_pthreadMutex);
			stringstream ss;
			ss << "ThreadPool: thread " << pthread_self() << " will exit";
			logInfo(ss.str());
			pthread_exit(NULL);
		}

		stringstream ss;
		ss << "ThreadPool: thread " << pthread_self() << " running";
		logInfo(ss.str());

		typename vector< Task<T>* >::iterator iter =tp->tasksList.begin();

		//deal with one task
		Task<T>* task = *iter;
		if (iter != tp->tasksList.end())
		{
			task = *iter;
			tp->tasksList.erase(iter);
		}

		pthread_mutex_unlock(&m_pthreadMutex);

		//conduct task and return generate taskResult
		T& v = task->run();
		//TaskResult<T> *tr = new TaskResult<T>(*task, v);
        tp->taskValue.push_back(v);


		ss << "ThreadPool: thread " << tid<< " is idle now";
		logInfo(ss.str());
	}
	return (void*)0;
}

//add task to tasks list and send thread sync signal
template <class T>
int ThreadPool <T>::addToThreadPool(Task<T> &task,int tid){
    pthread_mutex_lock(&m_pthreadMutex);
    this->tasksList.push_back(&task);
    this->taskId.push_back(tid);
    pthread_mutex_unlock(&m_pthreadMutex);
    pthread_cond_signal(&m_pthreadCond);
    return 0;
}

//create thread
template <class T>
int ThreadPool <T>::create(){
    pthread_id = (pthread_t*)malloc(sizeof(pthread_t) * taskLauched);
    for(int i = 0; i < taskLauched; i++)
    {
        pthread_create(&pthread_id[i], NULL, ThreadPool<T>::threadFunc, (void *)this);
    }
    return 0;
}

//stop all thread
template <class T>
int ThreadPool<T>::stopAll(){

    if (shutdown){
        return -1;
    }
    logInfo("ThreadPool: all thread running");

    //wake all threads before destroy threadpool
    shutdown = true;
    pthread_cond_broadcast(&m_pthreadCond);

    //wait for thread to exit
    for (int i = 0; i < taskLauched; i++)
    {
        pthread_join(pthread_id[i], NULL);
    }

    free(pthread_id);
    pthread_id = NULL;

    //free sync lock and condition variable
    pthread_mutex_destroy(&m_pthreadMutex);
    pthread_cond_destroy(&m_pthreadCond);

    return 0;
}

// get current ocuppy thread num
template <class T>
int ThreadPool<T>::getCurrentOccupyThreadNum()
{
    return tasksList.size();
}




#endif /* THREADPOOL_HPP_ */
