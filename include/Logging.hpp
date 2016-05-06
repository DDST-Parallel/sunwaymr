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

/*
 * initialization of static members
 */
int Logging::logging_mask = 1; // default logging mask is 1
pthread_mutex_t Logging::mutex_logging;

/*
 * set logging mask.
 *  0 or smaller    all logging messages are to be displayed
 *  1               debug, info, warning and error messages are to be displayed
 *  2               info, warning and error messages are to be displayed
 *  3               warning and error messages are to be displayed
 *  4               only error messages are to be displayed
 *  5 or higher     no logging messages are to be displayed
 */
void Logging::setMask(int m) {
	logging_mask = m;
}

/*
 * get the logging mask
 */
int Logging::getMask() {
	return logging_mask;
}

/*
 * logging verbose message
 */
void Logging::logVerbose(const string &msg) {
	if (logging_mask < 1) { // check logging mask
		pthread_mutex_lock(&mutex_logging);
		cout << "[" << currentDateTime() << "][VERBOSE]: " << msg << endl;
		pthread_mutex_unlock(&mutex_logging);
	}
}

/*
 * logging debug message
 */
void Logging::logDebug(const string &msg) {
	if (logging_mask < 2) { // check logging mask
		pthread_mutex_lock(&mutex_logging);
		cout << "[" << currentDateTime() << "][DEBUG]: " << msg << endl;
		pthread_mutex_unlock(&mutex_logging);
	}
}

/*
 * logging information message
 */
void Logging::logInfo(const string &msg) {
	if (logging_mask < 3) { // check logging mask
		pthread_mutex_lock(&mutex_logging);
		cout << "[" << currentDateTime() << "][INFO]: " << msg << endl;
		pthread_mutex_unlock(&mutex_logging);
	}
}

/*
 * logging warning message
 */
void Logging::logWarning(const string &msg) {
	if (logging_mask < 4) { // check logging mask
		pthread_mutex_lock(&mutex_logging);
		cout << "[" << currentDateTime() << "][WARNING]: " << msg << endl;
		pthread_mutex_unlock(&mutex_logging);
	}
}

/*
 * logging error message
 */
void Logging::logError(const string &msg) {
	if (logging_mask < 5) { // check logging mask
		pthread_mutex_lock(&mutex_logging);
		cout << "[" << currentDateTime() << "][ERROR]: " << msg << endl;
		pthread_mutex_unlock(&mutex_logging);
	}
}

#endif /* LOGGING_HPP_ */
