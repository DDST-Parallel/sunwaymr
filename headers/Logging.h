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

/*
 * Logging class.
 */
class Logging {
public:
	static void setMask(int m);
	static int getMask();
	static void logVerbose(const string &msg);
	static void logDebug(const string &msg);
	static void logInfo(const string &msg);
	static void logWarning(const string &msg);
	static void logError(const string &msg);

private:
	static int logging_mask;
	static pthread_mutex_t mutex_logging;
};


#endif /* LOGGING_H_ */
