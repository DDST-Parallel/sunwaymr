/*
 * RangeIteratorSeq.hpp
 *
 *  Created on: Jan 28, 2016
 *      Author: yupeng
 */

#ifndef INCLUDE_RANGEITERATORSEQ_HPP_
#define INCLUDE_RANGEITERATORSEQ_HPP_

#include "RangeIteratorSeq.h"

#include <assert.h>
#include "AbstractIteratorSeq.hpp"

template <class T> RangeIteratorSeq<T>::RangeIteratorSeq(T start, T end, T step)
: start(start), end(end), step(step), inclusive(true) {
}

template <class T> RangeIteratorSeq<T>::RangeIteratorSeq(T start, T end, T step, bool inclusive)
: start(start), end(end), step(step), inclusive(inclusive) {
}

template <class T> long RangeIteratorSeq<T>::size() {
	if (start == end) {
		if (inclusive) return 1;
		else return 0;
	}
	else {
		assert(step != 0);

		long ret = 1;
		ret += (end - start) / step;
		if ((end - start) % step == 0 && !inclusive) ret --;
		return ret;
	}
}

template <class T> T RangeIteratorSeq<T>::at(long index) {
	return start + index * step;
}

template <class T> vector<T> RangeIteratorSeq<T>:: getVector() {
	vector<T> ret;

	for (int i=0; i<this->size(); i++) {
		ret.push_back(this->at(i));
	}

	return ret;
}

template <class T> vector<T>& RangeIteratorSeq<T>::reduceLeft(T (*g)(T,T)) {
	vector<T> *ret = new vector<T>;

	if (size() > 0) {
		if (size() == 1) ret->push_back(start);
		else {
			T t = g(start, start + step);
			for(int i = 2; i < size(); i++) {
				t = g(t, start + step * i);
			}
			ret->push_back(t);
		}
	}

	return *ret;
}

#endif /* INCLUDE_RANGEITERATORSEQ_HPP_ */
