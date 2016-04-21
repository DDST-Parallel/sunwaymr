/*
 * SunwayMRContext.h
 *
 * To keep context information in whole process of RDD transformations.
 * First RDD in user app is created by parallelize, textFile, allNodes.
 * Additionally, runTasks can receive new tasks to run.
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef SUNWAYMRCONTEXT_H_
#define SUNWAYMRCONTEXT_H_

#include <string>

#include "IteratorSeq.h"
#include "AllNodesRDD.h"
#include "ParallelArrayRDD.h"
#include "TextFileRDD.h"
#include "JobScheduler.h"
#include "Task.h"
#include "TaskResult.h"
#include "FileSource.h"
#include "PointerContainer.h"
using std::string;

template <class T> class ParallelArrayRDD;
class TextFileRDD;

long SUNWAYMR_CONTEXT_ID = 0;

class SunwayMRContext {
public:
	SunwayMRContext();
	~SunwayMRContext();
	SunwayMRContext(string appName, int argc, char *argv[]);
	SunwayMRContext(string hostsFilePath, string master, string appName, int listenPort);
	void init(string hostsFilePath, string master, string appName, int listenPort);
	SunwayMRContext & operator=(const SunwayMRContext &c);

	// parallelize
	ParallelArrayRDD<int> * parallelize(int start, int end);
	ParallelArrayRDD<int> * parallelize(int start, int end, int numSlices);
	ParallelArrayRDD<long> * parallelize(long start, long end);
	ParallelArrayRDD<long> * parallelize(long start, long end, int numSlices);
	template <class T> ParallelArrayRDD<T> * parallelize(vector<T> &v);
	template <class T> ParallelArrayRDD<T> * parallelize(vector<T> &v, int numSlices);
	template <class T> ParallelArrayRDD<T> * parallelize(IteratorSeq<T> *iter);
	template <class T> ParallelArrayRDD<T> * parallelize(IteratorSeq<T> *iter, int numSlices);

	// textFile
	TextFileRDD * textFile(vector<FileSource> &files, FileSourceFormat format = FILE_SOURCE_FORMAT_BYTE);
	TextFileRDD * textFile(vector<FileSource> &files, int numSlices, FileSourceFormat format);

	// allNodes
	template <class T>
	AllNodesRDD<T> * allNodes(IteratorSeq< PointerContainer<T> > *seq); // all partitions will have the save seq

	template <class T> vector< TaskResult<T>* > runTasks(vector< Task<T>* > &tasks);

	string getMaster();
	int getListenPort();
	vector<string> getHosts();
	int getTotalThreads();

private:
	JobScheduler *scheduler;
	string hostsFilePath, master, appName;
	int listenPort;
	vector<string> hosts;

	void startScheduler();
};


#endif /* SUNWAYMRCONTEXT_H_ */
