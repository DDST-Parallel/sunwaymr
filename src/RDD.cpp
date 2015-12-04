#include "RDD.h"
#include "ReduceTask.h"
#include <iostream>
using namespace std;

template <class T>
RDD<T>::RDD(SunwayMRContext &c)
	: context(c)
{
}


template <class T> template <class U>
MappedRDD<U, T> RDD<T>::map(U (*f)(T))
{
	MappedRDD<U, T> map_rdd(*this, f);
	return map_rdd;
}

template <class T>
T RDD<T>::reduce(T (*g)(T, T))
{
	// construct tasks
	vector< ReduceTask<T> > tasks;
	for (int i = 0; i < partitions.size(); i++)
	{
		ReduceTask<T> task(*this, partitions[i], g);
		tasks.push_back(task);
	}
	// run tasks via context
	vector< TaskResult<T> > results = context.runTasks(tasks);
	//get results
	vector<T> values_results;
	for (int j = 0; j < results.size(); j++)
		values_results.push_back(results[j].value);

	if (values_results.size() == 0)
	{
		//should do logging
		cout << "empty collection!" << endl;
		return NULL;
	}
	//reduce left results
	IteratorSeq<T> it(values_results);
	return it.reduceLeft(g);
}
