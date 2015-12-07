/*
 * Logging.cpp
 *
 *  Created on: Dec 7, 2015
 *      Author: yupeng
 */

#include <iostream>
#include "Logging.h"
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

