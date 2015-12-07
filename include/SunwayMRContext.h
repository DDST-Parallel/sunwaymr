/*
 * SunwayMRContext.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef SUNWAYMRCONTEXT_H_
#define SUNWAYMRCONTEXT_H_

#include <string>
#include "ParallelArray.h"
#include "Task.h"
#include "TaskResult.h"
#include "Logging.h"
#include "Scheduler.h"
#include "IteratorSeq.h"
using std::string;

template <class T> class ParallelArray;

class SunwayMRContext : public Logging {
public:
	SunwayMRContext();
	SunwayMRContext(string appName, int argc, char *argv[]);
	SunwayMRContext(string hostsFilePath, string master, string appName, int listenPort);
	void init(string hostsFilePath, string master, string appName, int listenPort);

	ParallelArray<int> parallelize(int start, int end);
	ParallelArray<int> parallelize(int start, int end, int numSlices);
	ParallelArray<long> parallelize(long start, long end);
	ParallelArray<long> parallelize(long start, long end, int numSlices);
	template <class T> ParallelArray<T> parallelize(vector<T> &v);
	template <class T> ParallelArray<T> parallelize(vector<T> &v, int numSlices);
	template <class T> ParallelArray<T> parallelize(IteratorSeq<T> &iter);
	template <class T> ParallelArray<T> parallelize(IteratorSeq<T> &iter, int numSlices);

	template <class T> vector< TaskResult<T> > runTasks(vector< Task<T>* > &tasks);

private:
	Scheduler scheduler;

	string hostsFilePath, master, appName;
	int listenPort;

	void startScheduler();
};


#endif /* SUNWAYMRCONTEXT_H_ */
