#ifndef RDD_HPP_
#define RDD_HPP_

#include <ShuffledTask.hpp>
#include "RDD.h"

#include <iostream>
#include <sstream>

#include "ReduceTask.hpp"
#include "Task.hpp"
#include "TaskResult.hpp"
#include "VectorIteratorSeq.hpp"
#include "MappedRDD.hpp"
#include "FlatMappedRDD.hpp"
#include "PairRDD.hpp"
#include "Partition.hpp"
#include "SunwayMRContext.hpp"
#include "Logging.hpp"
#include "CollectTask.hpp"
#include "Pair.hpp"
#include "UnionRDD.hpp"
#include "VectorAutoPointer.hpp"
using namespace std;

template <class T>
RDD<T>::RDD(SunwayMRContext *c)
: context(c), sticky(false)
{
	rddID = XYZ_CURRENT_RDD_ID++;
}

template <class T>
RDD<T> & RDD<T>::operator=(const RDD<T> &r) {
	if(this == &r) {
		return *this;
	}

	this->deletePartitions();
	this->initRDDFrom(r);
	return *this;
}

template <class T>
void RDD<T>::initRDDFrom(const RDD<T> &r) {
	this->context = r.context;
	this->partitions = r.partitions;
	this->rddID = r.rddID;
}

template <class T>
RDD<T>::~RDD()
{
	this->deletePartitions();
	this->deleteIteratorSeqs();
}

template <class T>
bool RDD<T>::isSticky() {
	return sticky;
}

template <class T>
void RDD<T>::setSticky(bool s) {
	sticky = s;
}

template <class T>
void RDD<T>::deletePartitions() {
	for(unsigned int i = 0; i < this->partitions.size(); i++) {
		delete this->partitions[i];
	}
	this->partitions.clear();
}

template <class T>
void RDD<T>::deleteIteratorSeqs() {
	typename vector<IteratorSeq<T> *>::iterator it;
	for(it = this->iteratorSeqs.begin(); it != this->iteratorSeqs.end(); ++it) {
		delete (*it);
	}
	this->iteratorSeqs.clear();
}

template <class T> template <class U>
MappedRDD<U, T> * RDD<T>::map(U (*f)(T&))
{
	return new MappedRDD<U, T>(this, f);
}

template <class T> template <class U>
FlatMappedRDD<U, T> * RDD<T>::flatMap(vector<U> (*f)(T&))
{
	return new FlatMappedRDD<U, T>(this, f);
}

template <class T>
void RDD<T>::shuffle()
{
	// do nothing
}

template <class T> template <class K, class V>
PairRDD<K, V, T> * RDD<T>::mapToPair(Pair<K, V> (*f)(T&))
{
	return new PairRDD<K, V, T>(this, f);
}

template <class T>
T RDD<T>::reduce(T (*g)(T&, T&))
{
	this->shuffle();

	// construct tasks
	vector< Task< vector<T> >* > tasks;
	vector<Partition*> pars = this->getPartitions();
	for (unsigned int i = 0; i < pars.size(); i++)
	{
		Task< vector<T> > *task = new ReduceTask<T>(this, pars[i], g);
		tasks.push_back(task);
	}
	VectorAutoPointer< Task< vector<T> > > auto_ptr1(tasks);

	// run tasks via context
	vector< TaskResult< vector<T> >* > results = this->context->runTasks(tasks);
	VectorAutoPointer< TaskResult< vector<T> > > auto_ptr2(results);

	//get results
	vector<T> values_results;
	for (unsigned int j = 0; j < results.size(); j++)
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
	VectorIteratorSeq<T> it(values_results);
	return it.reduceLeft(g)[0];
}

template <class T>
Pair< T, int > distinct_inner_map_to_pair_f (T &t) {
	int i = 0;
	return Pair< T, int >(t, i);
}

template <class T>
Pair< T, int> distinct_inner_reduce_f (Pair< T, int > &p1, Pair< T, int > &p2) {
	int i = 0;
	return Pair< T, int >(p1.v1, i);
}

template <class T>
long distinct_inner_hash_f (Pair< T, int > &p) {
	stringstream ss;
	ss << p.v1;
	return hash(ss.str());
}

template <class T>
string distinct_inner_toString_f (Pair< T, int > &p) {
	stringstream ss;
	ss << p;
	return ss.str();
}

template <class T>
Pair< T, int > distinct_inner_fromString_f (string &s) {
	Pair< T, int > p;
	stringstream ss(s);
	ss >> p;
	return p;
}

template <class T>
T distinct_inner_map_f (Pair< T, int > &p) {
	return p.v1;
}

template <class T>
MappedRDD<T, Pair< T, int > > * RDD<T>::distinct(int newNumSlices) {
	return this->mapToPair(distinct_inner_map_to_pair_f<T>)
			->reduceByKey(distinct_inner_reduce_f<T>, newNumSlices)
			->map(distinct_inner_map_f<T>);
}

template <class T>
MappedRDD<T, Pair< T, int > > * RDD<T>::distinct() {
	return this->distinct(this->getPartitions().size());
}

template <class T>
vector<T> RDD<T>::collect()
{
	this->shuffle();
	vector<T> ret;

	// construct tasks
	vector< Task< vector<T> >* > tasks;
	vector<Partition*> pars = this->getPartitions();
	for(unsigned int i=0; i<pars.size(); i++)
	{
		Task< vector<T> > *task = new CollectTask<T>(this, pars[i]);
		tasks.push_back(task);
	}
	VectorAutoPointer< Task< vector<T> > > auto_ptr1(tasks); // delete pointers automatically

	// run tasks via context
	vector< TaskResult< vector<T> >* > results = this->context->runTasks(tasks);
	VectorAutoPointer< TaskResult< vector<T> > > auto_ptr2(results); // delete pointers automatically

	//get results
	for(unsigned int i=0; i<results.size(); i++)
	{
		for(unsigned int j=0; j<(results[i]->value).size(); j++)
		{
			ret.push_back((results[i]->value)[j]);
		}
	}
	return ret;
}

template <class T>
UnionRDD<T> * RDD<T>::unionRDD(RDD<T> *other) {
	vector< RDD<T>* > rdds;
	rdds.push_back(this);
	rdds.push_back(other);
	return new UnionRDD<T>(this->context, rdds);
}

#endif /* RDD_HPP_ */
