#include "ParallelArray.h"
#include<iostream>
using namespace std;

template <class T> long ParallelArray<T>::current_id = 1;

template <class T>
ParallelArray<T>::ParallelArray(SunwayMRContext &c, IteratorSeq<T> &seq, int numSlices)
	: RDD<T>::RDD (c) , seq(seq), numSlices(numSlices)
{
	parallelArray_id = current_id++;
	// get partitions
	vector<Partition> partitions;

	vector< IteratorSeq<T> > slices = slice();
	//construct partitions
	for (int i = 0; i < slices.size(); i++)
	{
		ParallelArrayPartition<T> partition(parallelArray_id, i, slices[i]);
		partitions.push_back(partition);
	}

	RDD<T>::partitions = partitions;
}

template <class T>
vector<Partition> ParallelArray<T>::getPartitions()
{
	return RDD<T>::partitions;
}

template <class T>
vector<string> ParallelArray<T>::preferredLocations(Partition &p)
{
	vector<string> list;
	return list;
}

template <class T>
IteratorSeq<T> ParallelArray<T>::iteratorSeq(Partition &p)
{
	ParallelArrayPartition<T> &pap = dynamic_cast<ParallelArrayPartition<T>&>(p);
	return pap.iteratorSeq();
}

template <class T>
vector< IteratorSeq<T> > ParallelArray<T>::slice()
{
	vector< IteratorSeq<T> > res;
	if (numSlices < 1)
	{
		cout << "slice number should be positive integer!" << endl;
		return res;
	}

	long num_group = seq.size() / numSlices;

	if (seq.type == 0)
	{
		for (int i = 0; i < numSlices - 1; i++)
		{
			long start = seq.getStart() + i * num_group * seq.getStep();
			long end = start + (num_group - 1) * seq.getStep();
			IteratorSeq<T> it(start, end , seq.getStep());
			res.push_back(it);
		}
		long last_start = seq.getStart() + (numSlices - 1) * num_group * seq.getStep();
		IteratorSeq<T> last(last_start, seq.getEnd(), seq.getStep());
		res.push_back(last);
	}
	else
	{
		// type == 1
		vector<T> data = seq.getVector();
		for (int i = 0; i < numSlices - 1; i++)
		{
			vector<T> group;
			for (long j = 0; j < num_group; j++)
			{
				group.push_back(data[i * num_group + j]);
			}
			IteratorSeq<T> it(group);
			res.push_back(it);
		}
		vector<T> last;
		for (long i = (numSlices - 1) * num_group; i < seq.size(); i++)
		{
			last.push_back(data[i]);
		}
		IteratorSeq<T> it(last);
		res.push_back(it);
	}

	return res;
}