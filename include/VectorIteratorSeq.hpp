/*
 * VectorIteratorSeq.hpp
 *
 *  Created on: Jan 28, 2016
 *      Author: yupeng
 */

#ifndef INCLUDE_VECTORITERATORSEQ_HPP_
#define INCLUDE_VECTORITERATORSEQ_HPP_

#include "VectorIteratorSeq.h"

#include <assert.h>
#include "AbstractIteratorSeq.hpp"

template <class T> VectorIteratorSeq<T>::VectorIteratorSeq(vector<T> &v)
: v(v){

}

template <class T> long VectorIteratorSeq<T>::size() {
	return v.size();
}

template <class T> T VectorIteratorSeq<T>::at(long index) {
	return v.at(index);
}

template <class T> vector<T> VectorIteratorSeq<T>::getVector() {
	return v;
}

template <class T> vector<T>& VectorIteratorSeq<T>::reduceLeft(T (*g)(T,T)) {
	vector<T> *ret = new vector<T>;

	if (v.size() > 0) {
		if (v.size() == 1) ret->push_back(v[0]);
		else {
			T t = g(v[0], v[1]);
			for(int i = 2; i < v.size(); i++) {
				t = g(t, v[i]);
			}
			ret->push_back(t);
		}
	}

	return *ret;
}

#endif /* INCLUDE_VECTORITERATORSEQ_HPP_ */
