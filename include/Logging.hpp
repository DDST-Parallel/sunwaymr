/*
 * Logging.hpp
 *
 *  Created on: Dec 7, 2015
 *      Author: yupeng
 */

#ifndef LOGGING_HPP_
#define LOGGING_HPP_


#include "Logging.h"

#include <iostream>
#include "Utils.hpp"
using std::cout;
using std::endl;

#ifndef LOGING_MASK
#define LOGING_MASK 0
#endif

pthread_mutex_t Logging::mutex_logging;

void Logging::logDebug(string msg) {
	if (LOGING_MASK < 1) {
		pthread_mutex_lock(&mutex_logging);
		cout << "[" << currentDateTime() << "] debug: " << msg << endl;
		pthread_mutex_unlock(&mutex_logging);
	}
}

void Logging::logInfo(string msg) {
	if (LOGING_MASK < 2) {
		pthread_mutex_lock(&mutex_logging);
		cout << "[" << currentDateTime() << "] info: " << msg << endl;
		pthread_mutex_unlock(&mutex_logging);
	}
}

void Logging::logWarning(string msg) {
	if (LOGING_MASK < 3) {
		pthread_mutex_lock(&mutex_logging);
		cout << "[" << currentDateTime() << "] warning: " << msg << endl;
		pthread_mutex_unlock(&mutex_logging);
	}
}

void Logging::logError(string msg) {
	if (LOGING_MASK < 4) {
		pthread_mutex_lock(&mutex_logging);
		cout << "[" << currentDateTime() << "] error: " << msg << endl;
		pthread_mutex_unlock(&mutex_logging);
	}
}



#endif /* LOGGING_HPP_ */
