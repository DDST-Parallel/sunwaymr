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
#define LOGING_MASK 1
#endif

Logging logger;

void Logging::logDebug(string msg) {
	if (LOGING_MASK < 1)
		cout << "[" << currentDateTime() << "] debug: " << msg << endl;
}

void Logging::logInfo(string msg) {
	if (LOGING_MASK < 2)
		cout << "[" << currentDateTime() << "] info: " << msg << endl;
}

void Logging::logWarning(string msg) {
	if (LOGING_MASK < 3)
		cout << "[" << currentDateTime() << "] warning: " << msg << endl;
}

void Logging::logError(string msg) {
	if (LOGING_MASK < 4)
		cout << "[" << currentDateTime() << "] error: " << msg << endl;
}



#endif /* LOGGING_HPP_ */
