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
#include "IteratorSeq.hpp"

template <class T> RangeIteratorSeq<T>::RangeIteratorSeq(T start, T end, T step)
: start(start), end(end), step(step), inclusive(true) {
}

template <class T> RangeIteratorSeq<T>::RangeIteratorSeq(T start, T end, T step, bool inclusive)
: start(start), end(end), step(step), inclusive(inclusive) {
}

template <class T> int RangeIteratorSeq<T>::getType() const {
	return 0;
}

template <class T> size_t RangeIteratorSeq<T>::size() const {
	if (start == end) {
		if (inclusive) return 1;
		else return 0;
	}
	else {
		assert(step != 0);

		size_t ret = 1;
		ret += (end - start) / step;
		if ((end - start) % step == 0 && !inclusive) ret --;
		return ret;
	}
}

template <class T> T RangeIteratorSeq<T>::at(size_t index) const {
	return start + index * step;
}

template <class T> vector<T> RangeIteratorSeq<T>:: getVector() const {
	vector<T> ret;

	for (size_t i=0; i<this->size(); i++) {
		ret.push_back(this->at(i));
	}

	return ret;
}

template <class T> vector<T> RangeIteratorSeq<T>::reduceLeft(T (*g)(T&, T&)) {
	vector<T> ret;

	if (size() > 0) {
		if (size() == 1) ret.push_back(start);
		else {
			T t2 = start + step;
			T t1 = g(start, t2);
			for(size_t i = 2; i < size(); i++) {
				t2 = start + step * i;
				t1 = g(t1, t2);
			}
			ret.push_back(t1);
		}
	}

	return ret;
}

#endif /* INCLUDE_RANGEITERATORSEQ_HPP_ */
