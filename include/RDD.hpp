#ifndef RDD_HPP_
#define RDD_HPP_

#include "RDD.h"

#include <iostream>

#include "ReduceTask.hpp"
#include "Task.hpp"
#include "IteratorSeq.hpp"
#include "MappedRDD.hpp"
#include "Partition.hpp"
#include "SunwayMRContext.hpp"
using namespace std;

template <class T>
RDD<T>::RDD()
: context(*(new SunwayMRContext())) {

}

template <class T>
RDD<T>::RDD(SunwayMRContext &c)
: context(c)
{

}

template <class T>
RDD<T>::~RDD()
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
	vector< Task< vector<T> >* > tasks;
	vector<Partition*> pars = getPartitions();

	for (int i = 0; i < pars.size(); i++)
	{
		Task< vector<T> > *task = new ReduceTask<T>(*this, *(pars[i]), g);
		tasks.push_back(task);
	}
	// run tasks via context
	vector< TaskResult< vector<T> >* > results = context.runTasks(tasks);
	//get results
	vector<T> values_results;
	for (int j = 0; j < results.size(); j++)
		if(results[j]->value.size() > 0) {
			values_results.push_back(results[j]->value[0]);
		}

	if (values_results.size() == 0)
	{
		//should do logging
		cout << "empty collection!" << endl;
		return NULL;
	}
	//reduce left results
	IteratorSeq<T> it(values_results);
	return it.reduceLeft(g)[0];
}


#endif /* RDD_HPP_ */
