/*
 * SunwayMRContext.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef SUNWAYMRCONTEXT_H_
#define SUNWAYMRCONTEXT_H_

#include <string>

#include "IteratorSeq.h"
#include "AllNodesRDD.h"
#include "ParallelArray.h"
#include "TextFileRDD.h"
#include "JobScheduler.h"
#include "Task.h"
#include "TaskResult.h"
#include "FileSource.h"
using std::string;

template <class T> class ParallelArray;
class TextFileRDD;


class SunwayMRContext {
public:
	SunwayMRContext();
	SunwayMRContext(string appName, int argc, char *argv[]);
	SunwayMRContext(string hostsFilePath, string master, string appName, int listenPort);
	void init(string hostsFilePath, string master, string appName, int listenPort);

	// parallelize
	ParallelArray<int> parallelize(int start, int end);
	ParallelArray<int> parallelize(int start, int end, int numSlices);
	ParallelArray<long> parallelize(long start, long end);
	ParallelArray<long> parallelize(long start, long end, int numSlices);
	template <class T> ParallelArray<T> parallelize(vector<T> &v);
	template <class T> ParallelArray<T> parallelize(vector<T> &v, int numSlices);
	template <class T> ParallelArray<T> parallelize(IteratorSeq<T> &iter);
	template <class T> ParallelArray<T> parallelize(IteratorSeq<T> &iter, int numSlices);

	// textFile
	TextFileRDD textFile(vector<FileSource> files);
	TextFileRDD textFile(vector<FileSource> files, int numSlices);

	// allNodes
	AllNodesRDD allNodes(IteratorSeq<void *> seq); // all partitions will have the save seq

	template <class T> vector< TaskResult<T>* > runTasks(vector< Task<T>* > &tasks);

	string getMaster();
	int getListenPort();
	vector<string> getHosts();
	int getTotalThreads();
	long getContextID();

private:
	JobScheduler *scheduler;

	string hostsFilePath, master, appName;
	int listenPort;
	vector<string> hosts;
	long contextID;

	void startScheduler();
};


#endif /* SUNWAYMRCONTEXT_H_ */
