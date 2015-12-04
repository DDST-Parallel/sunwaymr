/*
 * SunwayMRContext.cpp
 *
 *  Created on: Dec 4, 2015
 *      Author: yupeng
 */

#include "SunwayMRContext.h"

SunwayMRContext::SunwayMRContext(string hostsFilePath, string master, string appName)
: scheduler(Scheduler(hostsFilePath, master, appName)), hostsFilePath(hostsFilePath), master(master), appName(appName) {

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

template <class T> vector< TaskResult<T> > SunwayMRContext::runTasks(vector< Task<T> > &tasks) {
	return scheduler.runTasks(tasks);
}

