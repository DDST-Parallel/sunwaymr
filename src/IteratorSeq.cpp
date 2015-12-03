/*
 * IteratorSeq.cpp
 *
 *  Created on: Dec 3, 2015
 *      Author: yupeng
 */

#include "IteratorSeq.h"

template <class T> IteratorSeq<T>::IteratorSeq(T s, T end, T step, bool inclusive)
:start(s), end(end), step(step), inclusive(inclusive) {
	type = 0;
}

template <class T> IteratorSeq<T>::IteratorSeq(vector<T> &v)
		:v(v){
	type = 1;
	inclusive = false;
}

template <class T> long IteratorSeq<T>::size() {
	// TODO
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
	// TODO
}

template <class T> T IteratorSeq<T>::reduceLeft(T (*g)(T,T)) {
	// TODO
}


