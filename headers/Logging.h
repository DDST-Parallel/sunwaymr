/*
 * Logging.h
 *
 *  Created on: Dec 3, 2015
 *      Author: yupeng
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#include <string>
#include <exception>
using std::string;
using std::exception;

class Logging {
public:
	static void logInfo(string msg);
	static void logDebug(string msg);
	static void logWarning(string msg);
	static void logError(string msg);

//	void logInfo(string msg, exception e);
//	void logDebug(string msg, exception e);
//	void logWarning(string msg, exception e);
//	void logError(string msg, exception e);
};


#endif /* LOGGING_H_ */
