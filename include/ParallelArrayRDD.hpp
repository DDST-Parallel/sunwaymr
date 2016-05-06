#ifndef PARALLELARRAYRDD_HPP_
#define PARALLELARRAYRDD_HPP_


#include "ParallelArrayRDD.h"

#include <iostream>
#include <sstream>
#include <stdlib.h>

#include "IteratorSeq.hpp"
#include "RangeIteratorSeq.hpp"
#include "VectorIteratorSeq.hpp"
#include "RDD.hpp"
#include "Partition.hpp"
#include "SunwayMRContext.hpp"
#include "ParallelArrayPartition.hpp"
using namespace std;

/*
 * constructor
 */
template <class T>
ParallelArrayRDD<T>::ParallelArrayRDD(SunwayMRContext *c, IteratorSeq<T> *seq, int numSlices)
	: RDD<T>::RDD (c), numSlices(numSlices)
{
	parallelArray_id = RDD<T>::rddID;

	// get partitions
	vector<Partition *> partitions;
	vector< IteratorSeq<T>* > slices = slice(seq);

	if(slices.size() == 0) {
		exit(104);
	}

	// create partitions
	for (unsigned int i = 0; i < slices.size(); i++)
	{
		Partition* partition = new ParallelArrayPartition<T>(parallelArray_id, i, slices[i]);
		partitions.push_back(partition);
	}

	RDD<T>::partitions = partitions;
}

/*
 * to get partitions of this RDD
 */
template <class T>
vector<Partition*> ParallelArrayRDD<T>::getPartitions()
{
	return RDD<T>::partitions;
}

/*
 * to get preferred locations of a partition.
 * as to ParallelArrayPartition, there is no preferred locations.
 */
template <class T>
vector<string> ParallelArrayRDD<T>::preferredLocations(Partition *p)
{
	vector<string> list;
	return list;
}

/*
 * to get data in a partition
 */
template <class T>
IteratorSeq<T> * ParallelArrayRDD<T>::iteratorSeq(Partition *p)
{
	ParallelArrayPartition<T> *pap = dynamic_cast< ParallelArrayPartition<T> * >(p);
	return pap->iteratorSeq();
}

/*
 * split a IteratorSeq into a vector of IteratorSeqs for numbers of partitions
 */
template <class T>
vector< IteratorSeq<T>* > ParallelArrayRDD<T>::slice(IteratorSeq<T> *seq)
{
	vector< IteratorSeq<T>* > slices;
	if (numSlices < 1)
	{
		return slices;
	}

	long seqSize = seq->size();
	long num_group = seqSize / numSlices;

	if (seq->getType() == 0)
	{ // type == 0, split range
		T step = seq->at(1) - seq->at(0);
		for (int i = 0; i < numSlices - 1; i++)
		{
			T start = seq->at(0) + i * num_group * step;
			T end = start + (num_group - 1) * step;
			IteratorSeq<T> *it = new RangeIteratorSeq<T>(start, end , step);
			slices.push_back(it);
		}
		T last_start = seq->at(0) + (numSlices - 1) * num_group * step;
		T end = seq->at(seqSize-1);
		IteratorSeq<T> *last = new RangeIteratorSeq<T>(last_start, end, step);
		slices.push_back(last);
	}
	else
	{ // type == 1, split vector
		for (int i = 0; i < numSlices - 1; i++)
		{
			vector<T> group;
			VectorIteratorSeq<T> *it = new VectorIteratorSeq<T>(group);
			for (long j = 0; j < num_group; j++)
			{
				it->push_back(seq->at(i * num_group + j));
			}
			slices.push_back(it);
		}
		vector<T> last;
		VectorIteratorSeq<T> *it = new VectorIteratorSeq<T>(last);
		for (size_t i = (numSlices - 1) * num_group; i < seq->size(); i++)
		{
			it->push_back(seq->at(i));
		}
		slices.push_back(it);
	}

	return slices;
}


#endif /* PARALLELARRAYRDD_HPP_ */
