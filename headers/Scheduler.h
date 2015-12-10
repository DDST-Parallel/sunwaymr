/*
 * Scheduler.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <vector>

#include "Messaging.h"
#include "Logging.h"

using std::vector;


class Scheduler : public Messaging, public Logging {
public:
	Scheduler();
	virtual ~Scheduler();
	virtual void messageReceived(int localListenPort, string fromHost, int msgType, string msg) = 0;

};


#endif /* SCHEDULER_H_ */
