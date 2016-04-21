/*
 * Scheduler.h
 *
 * Super class of TaskScheduler
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <string>

using std::string;


class Scheduler {
public:
	Scheduler();
	virtual ~Scheduler();
	virtual void handleMessage(int localListenPort, string fromHost, int msgType, string &msg, int &retValue) = 0;

};


#endif /* SCHEDULER_H_ */
