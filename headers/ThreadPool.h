/*
 * ThreadPool.h
 *
 *  Created on: Dec 7, 2015
 *      Author: shi qiuwei
 */

#ifndef THREAD_POOL_H_
#define THREAD_POOL_H_


class ThreadPool {
public:
	ThreadPool(int threadNum=10);
	void run(void* (*func)(void* data));

};


#endif /* THREAD_POOL_H_ */
