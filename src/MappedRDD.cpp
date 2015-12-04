#include "MappedRDD.h"
#include <iostream>
using namespace std;

template <class U, class T>
MappedRDD<U, T>::MappedRDD(RDD<T> &prev, U (*f)(T))
	: prevRDD(prev)
{
	mappedFunction = f;
}

template <class U, class T>
vector<Partition> MappedRDD<U, T>::getPartitions()
{
	return prevRDD.getPartitions();
}

template <class U, class T>
vector<string> MappedRDD<U, T>::preferredLocations(Partition &p)
{
	return prevRDD.preferredLocations(p);
}

template <class U, class T>
IteratorSeq<U>  MappedRDD<U, T>::iteratorSeq(Partition &p)
{
	IteratorSeq<T> seq1 = prevRDD.iteratorSeq(p);
	return seq1.map(mappedFunction);
}