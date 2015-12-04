#include "ParallelArrayPartition.h"
#include <iostream>
using namespace std;

template <class T>
ParallelArrayPartition<T>::ParallelArrayPartition(long _rddID, int _partitionID, IteratorSeq<T> &_values) 
: values(_values), rddID(_rddID), partitionID(_partitionID)
{
}

template <class T>
IteratorSeq<T> ParallelArrayPartition<T>::iteratorSeq()
{
	return values;
}

