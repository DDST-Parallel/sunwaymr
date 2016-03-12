/*
 * Logging.h
 *
 *  Created on: Dec 3, 2015
 *      Author: yupeng
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#include <string>
#include <pthread.h>
using std::string;
using std::exception;

class Logging {
public:
	static void setMask(int m);
	static void logVerbose(string msg);
	static void logDebug(string msg);
	static void logInfo(string msg);
	static void logWarning(string msg);
	static void logError(string msg);

//	void logInfo(string msg, exception e);
//	void logDebug(string msg, exception e);
//	void logWarning(string msg, exception e);
//	void logError(string msg, exception e);

private:
	static int logging_mask;
	static pthread_mutex_t mutex_logging;
};


#endif /* LOGGING_H_ */
