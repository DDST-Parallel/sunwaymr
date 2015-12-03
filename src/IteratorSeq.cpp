/*
 * IteratorSeq.cpp
 *
 *  Created on: Dec 3, 2015
 *      Author: yupeng
 */

#include <assert.h>
#include "IteratorSeq.h"

template <class T> IteratorSeq<T>::IteratorSeq(T start, T end, T step)
: start(start), end(end), step(step), inclusive(true) {
	type = 0;
}

template <class T> IteratorSeq<T>::IteratorSeq(T start, T end, T step, bool inclusive)
: start(start), end(end), step(step), inclusive(inclusive) {
	type = 0;
}

template <class T> IteratorSeq<T>::IteratorSeq(vector<T> &v)
		:v(v){
	type = 1;
	inclusive = false;
}

template <class T> long IteratorSeq<T>::size() {
	if (type == 0) {
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
	else {
		return v.size();
	}
}

template <class T> T IteratorSeq<T>::getStart() {
	return start;
}

template <class T> T IteratorSeq<T>::getEnd() {
	return end;
}

template <class T> T IteratorSeq<T>::getStep() {
	return step;
}

template <class T> bool IteratorSeq<T>::isInclusive() {
	return inclusive;
}

template <class T> vector<T> IteratorSeq<T>::getVector() {
	return v;
}

template <class T> template <class U> IteratorSeq<U> IteratorSeq<T>::map(U (*f)(T)) {
	// TODO map concurrently

	vector<U> ret;

	if (type == 0) {
		for(int i = 0; i < size(); i++) {
			T t = start + step * i;
			U u = f(t);
			ret.push_back(u);
		}
	} else {
		for(int i = 0; i < v.size(); i++) {
			U u = f(v[i]);
			ret.push_back(u);
		}
	}

	return IteratorSeq<U>(ret);
}

template <class T> vector<T> IteratorSeq<T>::reduceLeft(T (*g)(T,T)) {
	vector<T> ret;

	if(type == 0) {
		if (size() > 0) {
			if (size() == 1) ret.push_back(start);
			else {
				T t = g(start, start + step);
				for(int i = 2; i < size(); i++) {
					t = g(t, start + step * i);
				}
				ret.push_back(t);
			}
		}

	} else {
		if (v.size() > 0) {
			if (v.size() == 1) ret.push_back(v[0]);
			else {
				T t = g(v[0], v[1]);
				for(int i = 2; i < v.size(); i++) {
					t = g(t, v[i]);
				}
				ret.push_back(t);
			}
		}
	}

	return ret;
}


