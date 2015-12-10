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
using std::cout;
using std::endl;

void Logging::logInfo(string msg) {
	cout << "info: " << msg << endl;
}

void Logging::logDebug(string msg) {
	cout << "debug: " << msg << endl;
}

void Logging::logWarning(string msg) {
	cout << "warning: " << msg << endl;
}

void Logging::logError(string msg) {
	cout << "error: " << msg << endl;
}



#endif /* LOGGING_HPP_ */
