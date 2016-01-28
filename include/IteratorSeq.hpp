/*
 * IteratorSeq.hpp
 *
 *  Created on: Dec 3, 2015
 *      Author: yupeng
 */

#ifndef ITERATORSEQ_HPP_
#define ITERATORSEQ_HPP_

#include "IteratorSeq.h"

#include <assert.h>
#include "AbstractIteratorSeq.hpp"
#include "RangeIteratorSeq.hpp"
#include "VectorIteratorSeq.hpp"

template <class T> IteratorSeq<T>::IteratorSeq(T start, T end, T step)
{
	iterator = new RangeIteratorSeq<T>(start, end, step);
	type = 0;
}

template <class T> IteratorSeq<T>::IteratorSeq(T start, T end, T step, bool inclusive)
{
	iterator = new RangeIteratorSeq<T>(start, end, step, inclusive);
	type = 0;
}

template <class T> IteratorSeq<T>::IteratorSeq(vector<T> &v)
{
	iterator = new VectorIteratorSeq<T>(v);
	type = 1;
}

template <class T> IteratorSeq<T>::IteratorSeq(RangeIteratorSeq<T> *r) {
	iterator = r;
	type = 0;
}
template <class T> IteratorSeq<T>::IteratorSeq(VectorIteratorSeq<T> *v) {
	iterator = v;
	type = 1;
}

template <class T> long IteratorSeq<T>::size() {
	return iterator->size();
}

template <class T> T IteratorSeq<T>::at(long index) {
	return iterator->at(index);
}

template <class T> vector<T> IteratorSeq<T>::getVector() {
	return iterator->getVector();
}

template <class T> template <class U> IteratorSeq<U> IteratorSeq<T>::map(U (*f)(T)) {
	return IteratorSeq<U>(iterator->map(f));
}

template <class T> template <class U> IteratorSeq<U> IteratorSeq<T>::flatMap(vector<U> (*f)(T)) {
	return IteratorSeq<U>(iterator->flatMap(f));
}

template <class T> vector<T>& IteratorSeq<T>::reduceLeft(T (*g)(T,T)) {
	return iterator->reduceLeft(g);
}

#endif /* ITERATORSEQ_HPP_ */
