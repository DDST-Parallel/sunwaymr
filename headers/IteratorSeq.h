/*
 * IteratorSeq.h
 *
 * Just like Collection in Java
 * Return type of RDD::iteratorSeq
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef ITERATORSEQ_H_
#define ITERATORSEQ_H_

#include <vector>
#include <iostream>
#include "AbstractIteratorSeq.h"
#include "RangeIteratorSeq.h"
#include "VectorIteratorSeq.h"
using std::vector;
using std::istream;
using std::ostream;

#ifndef ITERATORSEQ_DELIMITATION_LEFT
#define ITERATORSEQ_DELIMITATION_LEFT "\a(\a"
#endif
#ifndef ITERATORSEQ_DELIMITATION_RIGHT
#define ITERATORSEQ_DELIMITATION_RIGHT "\a)\a"
#endif

template <class T>
class IteratorSeq {
public:
	IteratorSeq();
	IteratorSeq(T start, T end, T step);
	IteratorSeq(T start, T end, T step, bool inclusive);
	IteratorSeq(vector<T> &v);
	IteratorSeq(RangeIteratorSeq<T> *r);
	IteratorSeq(VectorIteratorSeq<T> *v);
	~IteratorSeq();
	void init(vector<T> &v);
	int type; // 0: range, 1: vector

	long size() const;
	T at(long index) const;
	vector<T> getVector();
	template <class U> IteratorSeq<U> map(U (*f)(T));
	template <class U> IteratorSeq<U> flatMap(vector<U> (*f)(T));
	vector<T>& reduceLeft(T (*g)(T,T));
	template <class U>
	friend ostream& operator<< (ostream &out, const IteratorSeq<U> &s);
	template <class U>
	friend istream& operator>> (istream &in, IteratorSeq<U> &s);

private:
	AbstractIteratorSeq<T> *iterator;
};

#endif /* ITERATORSEQ_H_ */
