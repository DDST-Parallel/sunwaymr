/*
 * SunwayMRContext.cpp
 *
 *  Created on: Dec 4, 2015
 *      Author: yupeng
 */

#include <stdlib.h>
#include "SunwayMRContext.h"

SunwayMRContext::SunwayMRContext(string appName, int argc, char *argv[])
: appName(appName) {
	if (argc < 3) {
		// error
		logError("3 parameters at least \nhostsFile, master, listenPort");
		exit(101);

	} else {
		hostsFilePath = string(argv[1]);
		master = string(argv[2]);
		listenPort = atoi(argv[3]);

		scheduler = Scheduler(hostsFilePath, master, appName, listenPort);

		startScheduler();

	}
}

SunwayMRContext::SunwayMRContext(string hostsFilePath, string master, string appName, int listenPort)
: scheduler(Scheduler(hostsFilePath, master, appName, listenPort)),
  hostsFilePath(hostsFilePath), master(master), appName(appName),
  listenPort(listenPort) {
	startScheduler();

}

void SunwayMRContext::startScheduler() {
	bool r = scheduler.start();

	if (!r) {
		logError("failed to start scheduler, send port or listen port may be in use.");
		exit(102);
	}
}

ParallelArray<int> SunwayMRContext::parallelize(int start, int end) {
	return parallelize(IteratorSeq<int>(start, end, 1), scheduler.totalThreads());
}

ParallelArray<int> SunwayMRContext::parallelize(int start, int end, int numSlices) {
	return parallelize(IteratorSeq<int>(start, end, 1), numSlices);
}

ParallelArray<long> SunwayMRContext::parallelize(long start, long end) {
	return parallelize(IteratorSeq<long>(start, end, 1), scheduler.totalThreads());
}

ParallelArray<long> SunwayMRContext::parallelize(long start, long end, int numSlices) {
	return parallelize(IteratorSeq<long>(start, end, 1), numSlices);
}

template <class T> ParallelArray<T> SunwayMRContext::parallelize(vector<T> &v) {
	return parallelize(IteratorSeq<T>(v), scheduler.totalThreads());
}

template <class T> ParallelArray<T> SunwayMRContext::parallelize(vector<T> &v, int numSlices){
	return parallelize(IteratorSeq<T>(v), numSlices);
}

template <class T> ParallelArray<T> SunwayMRContext::parallelize(IteratorSeq<T> iter) {
	return parallelize(iter, scheduler.totalThreads());
}

template <class T> ParallelArray<T> SunwayMRContext::parallelize(IteratorSeq<T> iter, int numSlices) {
	return ParallelArray<T>(*this, iter, numSlices);
}

template <class T> vector< TaskResult<T> > SunwayMRContext::runTasks(vector< Task<T>* > &tasks) {
	return scheduler.runTasks(tasks);
}

