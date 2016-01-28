/*
 * IteratorSeq.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef ITERATORSEQ_H_
#define ITERATORSEQ_H_

#include <vector>
#include "AbstractIteratorSeq.h"
#include "RangeIteratorSeq.h"
#include "VectorIteratorSeq.h"
using std::vector;

template <class T>
class IteratorSeq {
public:
	IteratorSeq(T start, T end, T step);
	IteratorSeq(T start, T end, T step, bool inclusive);
	IteratorSeq(vector<T> &v);
	IteratorSeq(RangeIteratorSeq<T> *r);
	IteratorSeq(VectorIteratorSeq<T> *v);
	int type; // 0: range, 1: vector

	long size();
	T at(long index);
	vector<T> getVector();
	template <class U> IteratorSeq<U> map(U (*f)(T));
	template <class U> IteratorSeq<U> flatMap(vector<U> (*f)(T));
	vector<T>& reduceLeft(T (*g)(T,T));

private:
	AbstractIteratorSeq<T> *iterator;
};

#endif /* ITERATORSEQ_H_ */
