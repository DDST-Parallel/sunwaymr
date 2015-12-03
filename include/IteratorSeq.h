/*
 * IteratorSeq.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef ITERATORSEQ_H_
#define ITERATORSEQ_H_

#include <vector>
using std::vector;

template <class T>
class IteratorSeq {
public:
	IteratorSeq(T start, T end, T step, bool inclusive = false);
	IteratorSeq(vector<T> &v);
	int type; // 0: range, 1: vector
	long size();
	T getStart();
	T getEnd();
	T getStep();
	bool isInclusive();
	vector<T> getVector();
	template <class U> IteratorSeq<U> map(U (*f)(T));
	T reduceLeft(T (*g)(T,T));

private:
	T start;
	T end;
	T step;
	bool inclusive;
	vector<T> v;

};

#endif /* ITERATORSEQ_H_ */
