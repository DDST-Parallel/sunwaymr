/*
 * SunwayMRContext.hpp
 *
 *  Created on: Dec 4, 2015
 *      Author: yupeng
 */

#ifndef SUNWAYMRCONTEXT_HPP_
#define SUNWAYMRCONTEXT_HPP_


#include "SunwayMRContext.h"

#include <stdlib.h>

#include "IteratorSeq.hpp"
#include "JobScheduler.hpp"
#include "Logging.hpp"
#include "AllNodesRDD.hpp"
#include "ParallelArrayRDD.hpp"
#include "TextFileRDD.hpp"
#include "Task.hpp"
#include "TaskResult.hpp"
#include "FileSource.hpp"
#include "RangeIteratorSeq.hpp"

SunwayMRContext::SunwayMRContext() {
	hostsFilePath = "";
	master = "";
	listenPort = 0;
	hosts = vector<string>();
	scheduler = new JobScheduler();
	SUNWAYMR_CONTEXT_ID = getCurrentTime();
}

SunwayMRContext::~SunwayMRContext() {
	delete this->scheduler;
}

SunwayMRContext::SunwayMRContext(string appName, int argc, char *argv[])
: appName(appName) {
	if (argc < 4) {
		// error
		Logging::logError("SunwayMRContext: 3 parameters at least: hostsFile, master, listenPort");
		exit(101);

	} else {
		hostsFilePath = string(argv[1]);
		master = string(argv[2]);
		listenPort = atoi(argv[3]);
		hosts = vector<string>();

		scheduler = new JobScheduler(hostsFilePath, master, appName, listenPort);
		SUNWAYMR_CONTEXT_ID = getCurrentTime();
		startScheduler();

	}
}

SunwayMRContext::SunwayMRContext(string hostsFilePath, string master, string appName, int listenPort)
: hostsFilePath(hostsFilePath), master(master), appName(appName),
  listenPort(listenPort) {

	hosts = vector<string>();
	scheduler = new JobScheduler(hostsFilePath, master, appName, listenPort);
	SUNWAYMR_CONTEXT_ID = getCurrentTime();
	startScheduler();

}

void SunwayMRContext::init(string hostsFilePath, string master, string appName, int listenPort) {
	this->hostsFilePath = hostsFilePath;
	this->master = master;
	this->appName = appName;
	this->listenPort = listenPort;

	this->hosts = vector<string>();
	scheduler = new JobScheduler(hostsFilePath, master, appName, listenPort);
	startScheduler();

}

SunwayMRContext & SunwayMRContext::operator=(const SunwayMRContext &c) {
	if(this == &c) {
		return *this;
	}
	this->hostsFilePath = c.hostsFilePath;
	this->master = c.master;
	this->appName = c.appName;
	this->listenPort = c.listenPort;
	this->hosts = c.hosts;
	this->scheduler = c.scheduler;
	return *this;
}

void SunwayMRContext::startScheduler() {
	Logging::logInfo("SunwayMRContext: starting scheduler...");
	bool r = scheduler->start();
	if (!r) {
		Logging::logError("SunwayMRContext: failed to start scheduler, listen port may be in use.");
		exit(102);
	}
	Logging::logInfo("SunwayMRContext: starting scheduler succeeded");
}

ParallelArrayRDD<int> * SunwayMRContext::parallelize(int start, int end) {
	int step = (start > end) ? -1 : 1;
	IteratorSeq<int> *iter = new RangeIteratorSeq<int>(start, end, step);
	return parallelize<int>(iter, scheduler->totalThreads());
}

ParallelArrayRDD<int> * SunwayMRContext::parallelize(int start, int end, int numSlices) {
	int step = (start > end) ? -1 : 1;
	IteratorSeq<int> *iter = new RangeIteratorSeq<int>(start, end, step);
	return parallelize<int>(iter, numSlices);
}

ParallelArrayRDD<long> * SunwayMRContext::parallelize(long start, long end) {
	long step = (start > end) ? -1l : 1l;
	IteratorSeq<long> *iter = new RangeIteratorSeq<long>(start, end, step);
	return parallelize<long>(iter, scheduler->totalThreads());
}

ParallelArrayRDD<long> * SunwayMRContext::parallelize(long start, long end, int numSlices) {
	long step = (start > end) ? -1l : 1l;
	IteratorSeq<long> *iter = new RangeIteratorSeq<long>(start, end, step);
	return parallelize<long>(iter, numSlices);
}

template <class T> ParallelArrayRDD<T> * SunwayMRContext::parallelize(vector<T> &v) {
	IteratorSeq<T> *iter = new VectorIteratorSeq<T>(v);
	return parallelize<T>(iter, scheduler->totalThreads());
}

template <class T> ParallelArrayRDD<T> * SunwayMRContext::parallelize(vector<T> &v, int numSlices){
	IteratorSeq<T> *iter = new VectorIteratorSeq<T>(v);
	return parallelize<T>(iter, numSlices);
}

template <class T> ParallelArrayRDD<T> * SunwayMRContext::parallelize(IteratorSeq<T> *iter) {
	return parallelize<T>(iter, scheduler->totalThreads());
}

// actual parallelizer
template <class T> ParallelArrayRDD<T> * SunwayMRContext::parallelize(IteratorSeq<T> *iter, int numSlices) {
	if (numSlices < 1)
	{
		Logging::logError("SunwayMRContext: slice number should be positive integer!");
		exit(104);
	}
	return new ParallelArrayRDD<T>(this, iter, numSlices);
}

// textFile
TextFileRDD * SunwayMRContext::textFile(vector<FileSource> &files, FileSourceFormat format) {
	return textFile(files, scheduler->totalThreads(), format);
}

TextFileRDD * SunwayMRContext::textFile(vector<FileSource> &files, int numSlices, FileSourceFormat format) {
	if (numSlices < 1)
	{
		Logging::logError("SunwayMRContext: slice number should be positive integer!");
		exit(104);
	}
	return new TextFileRDD(this, files, numSlices, format);
}

// allNodes
template <class T>
AllNodesRDD<T> * SunwayMRContext::allNodes(IteratorSeq< PointerContainer<T> > *seq) {
	return new AllNodesRDD<T>(this, seq);
}

template <class T> vector< TaskResult<T>* > SunwayMRContext::runTasks(vector< Task<T>* > &tasks) {
	if (tasks.size() == 0) {
		Logging::logError("SunwayMRContext: runTasks: tasks zero size");
	}
	return scheduler->runTasks(tasks);
}

string SunwayMRContext::getMaster() {
	return master;
}

int SunwayMRContext::getListenPort() {
	return listenPort;
}

vector<string> SunwayMRContext::getHosts() {
	if (hosts.size() > 0) return hosts;
	return scheduler->getHosts();
}

int SunwayMRContext::getTotalThreads() {
	return scheduler->totalThreads();
}

#endif /* SUNWAYMRCONTEXT_HPP_ */
