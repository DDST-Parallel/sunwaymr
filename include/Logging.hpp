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

int Logging::logging_mask = 1;
pthread_mutex_t Logging::mutex_logging;

void Logging::setMask(int m) {
	logging_mask = m;
}

void Logging::logVerbose(const string &msg) {
	if (logging_mask < 1) {
		pthread_mutex_lock(&mutex_logging);
		cout << "[" << currentDateTime() << "][VERBOSE]: " << msg << endl;
		pthread_mutex_unlock(&mutex_logging);
	}
}

void Logging::logDebug(const string &msg) {
	if (logging_mask < 2) {
		pthread_mutex_lock(&mutex_logging);
		cout << "[" << currentDateTime() << "][DEBUG]: " << msg << endl;
		pthread_mutex_unlock(&mutex_logging);
	}
}

void Logging::logInfo(const string &msg) {
	if (logging_mask < 3) {
		pthread_mutex_lock(&mutex_logging);
		cout << "[" << currentDateTime() << "][INFO]: " << msg << endl;
		pthread_mutex_unlock(&mutex_logging);
	}
}

void Logging::logWarning(const string &msg) {
	if (logging_mask < 4) {
		pthread_mutex_lock(&mutex_logging);
		cout << "[" << currentDateTime() << "][WARNING]: " << msg << endl;
		pthread_mutex_unlock(&mutex_logging);
	}
}

void Logging::logError(const string &msg) {
	if (logging_mask < 5) {
		pthread_mutex_lock(&mutex_logging);
		cout << "[" << currentDateTime() << "][ERROR]: " << msg << endl;
		pthread_mutex_unlock(&mutex_logging);
	}
}



#endif /* LOGGING_HPP_ */
