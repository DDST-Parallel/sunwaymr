#ifndef RDD_HPP_
#define RDD_HPP_

#include "RDD.h"

#include <iostream>

#include "ReduceTask.hpp"
#include "Task.hpp"
#include "TaskResult.hpp"
#include "IteratorSeq.hpp"
#include "MappedRDD.hpp"
#include "FlatMappedRDD.hpp"
#include "PairRDD.hpp"
#include "Partition.hpp"
#include "SunwayMRContext.hpp"
#include "Logging.hpp"
#include "CollectTask.hpp"
#include "ShuffleTask.hpp"
using namespace std;

template <class T> long RDD<T>::current_id = 1;

template <class T>
RDD<T>::RDD(SunwayMRContext &c)
: context(c)
{
	rddID = current_id++;
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

template <class T> template <class U>
FlatMappedRDD<U, T> RDD<T>::flatMap(vector<U> (*f)(T))
{
	FlatMappedRDD<U, T> map_rdd(*this, f);
	return map_rdd;
}

template <class T>
void RDD<T>::shuffle()
{
	// do nothing
}

template <class T> template <class K, class V>
PairRDD<K, V, T> RDD<T>::mapToPair(Pair<K, V> (*f)(T))
{
	PairRDD<K, V, T> pair_rdd(*this, f);
	return pair_rdd;
}

template <class T>
T RDD<T>::reduce(T (*g)(T, T))
{
	shuffle();
	// construct tasks
	vector< Task< vector<T> >* > tasks;
	vector<Partition*> pars = this->getPartitions();

	for (int i = 0; i < pars.size(); i++)
	{
		Task< vector<T> > *task = new ReduceTask<T>(*this, *(pars[i]), g);
		tasks.push_back(task);
	}

	// run tasks via context
	vector< TaskResult< vector<T> >* > results = this->context.runTasks(tasks);

	//get results
	vector<T> values_results;
	for (int j = 0; j < results.size(); j++)
		if(results[j]->value.size() > 0) {
			values_results.push_back(results[j]->value[0]);
		}

	if (values_results.size() == 0)
	{
		//should do logging
		Logging::logWarning("RDD: reduce received empty results collection!!!");
		return 0;
	}
	//reduce left results
	IteratorSeq<T> it(values_results);
	return it.reduceLeft(g)[0];
}

template <class T>
vector<T>& RDD<T>::collect()
{
	shuffle();
	vector<T> *ret = new vector<T>;
	// construct tasks
	vector< Task< vector<T> >* > tasks;
	vector<Partition*> pars = this->getPartitions();
	for(int i=0; i<pars.size(); i++)
	{
		Task< vector<T> > *task = new CollectTask<T>(*this, *(pars[i]));
		tasks.push_back(task);
	}

	// run tasks via context
	vector< TaskResult< vector<T> >* > results = this->context.runTasks(tasks);
	//get results
	for(int i=0; i<results.size(); i++)
	{
		for(int j=0; j<(results[i]->value).size(); j++)
		{
			ret->push_back((results[i]->value)[j]);
		}
	}
	return *ret;
}

#endif /* RDD_HPP_ */
