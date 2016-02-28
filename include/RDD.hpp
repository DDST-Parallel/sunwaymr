#ifndef RDD_HPP_
#define RDD_HPP_

#include "RDD.h"

#include <iostream>
#include <sstream>

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
#include "Pair.hpp"
#include "UnionRDD.hpp"
using namespace std;

template <class T>
RDD<T>::RDD(SunwayMRContext &c)
: context(c)
{
	rddID = XYZ_CURRENT_RDD_ID++;
}

template <class T>
RDD<T> & RDD<T>::operator=(const RDD<T> &p) {
	this->initRDDFrom(p);
	return *this;
}

template <class T>
RDD<T>::~RDD()
{

}

template <class T>
void RDD<T>::initRDDFrom(const RDD<T> &r) {
	this->context = r.context;
	this->partitions = r.partitions;
	this->rddID = r.rddID;
}

template <class T> template <class U>
MappedRDD<U, T> & RDD<T>::map(U (*f)(T))
{
	MappedRDD<U, T> *map_rdd = new MappedRDD<U, T>(*this, f);
	return *map_rdd;
}

template <class T> template <class U>
FlatMappedRDD<U, T> & RDD<T>::flatMap(vector<U> (*f)(T))
{
	FlatMappedRDD<U, T> *map_rdd = new FlatMappedRDD<U, T>(*this, f);
	return *map_rdd;
}

template <class T>
void RDD<T>::shuffle()
{
	// do nothing
}

template <class T> template <class K, class V>
PairRDD<K, V, T> & RDD<T>::mapToPair(Pair<K, V> (*f)(T))
{
	PairRDD<K, V, T> *pair_rdd = new PairRDD<K, V, T>(*this, f);
	return *pair_rdd;
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
Pair< T, int > distinct_inner_map_to_pair_f (T t) {
	return Pair< T, int >(t, 0);
}

template <class T>
Pair< T, int> distinct_inner_reduce_f (Pair< T, int > p1, Pair< T, int > p2) {
	return Pair< T, int >(p1.v1, 0);
}

template <class T>
long distinct_inner_hash_f (Pair< T, int > p) {
	stringstream ss;
	ss << p.v1;
	return hash(ss.str());
}

template <class T>
string distinct_inner_toString_f (Pair< T, int > p) {
	stringstream ss;
	ss << p;
	return ss.str();
}

template <class T>
Pair< T, int > distinct_inner_fromString_f (string s) {
	Pair< T, int > p;
	stringstream ss(s);
	ss >> p;
	return p;
}

template <class T>
T distinct_inner_map_f (Pair< T, int > p) {
	return p.v1;
}

template <class T>
MappedRDD<T, Pair< T, int > > & RDD<T>::distinct(int newNumSlices) {
    //map(x => (x, null)).reduceByKey((x, y) => x, numPartitions).map(_._1)
	return this->mapToPair(distinct_inner_map_to_pair_f<T>)
			.reduceByKey(distinct_inner_reduce_f<T>, newNumSlices)
			.map(distinct_inner_map_f<T>);
}

template <class T>
MappedRDD<T, Pair< T, int > > & RDD<T>::distinct() {
	return this->distinct(this->getPartitions().size());
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

template <class T>
UnionRDD<T> & RDD<T>::unionRDD(RDD<T> *other) {
	vector< RDD<T>* > rdds;
	rdds.push_back(this);
	rdds.push_back(other);
	UnionRDD<T> *ret = new UnionRDD<T>(this->context, rdds);
	return *ret;
}

#endif /* RDD_HPP_ */
