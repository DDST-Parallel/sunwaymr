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
#include <algorithm>
#include "IteratorSeq.hpp"
#include "Utils.hpp"

/*
 * default constructor
 */
template <class T> VectorIteratorSeq<T>::VectorIteratorSeq()
{

}

/*
 * constructor from a vector
 */
template <class T> VectorIteratorSeq<T>::VectorIteratorSeq(vector<T> &v)
:v(v)
{

}

/*
 * removing all elements
 */
template <class T> void VectorIteratorSeq<T>::clear() {
	this->v.clear();
}

/*
 * push_back an element
 */
template <class T> void VectorIteratorSeq<T>::push_back(T t) {
	this->v.push_back(t);
}

/*
 * push_back a vector of elements
 */
template <class T> void VectorIteratorSeq<T>::push_back(vector<T> &v) {
	this->v.insert(this->v.end(), v.begin(), v.end());
}

/*
 * to reserve the capacity of vector
 */
template <class T> void VectorIteratorSeq<T>::reserve(size_t size) {
	this->v.reserve(size);
}

/*
 * to get the type of IteratorSeq.
 * return 1.
 */
template <class T> int VectorIteratorSeq<T>::getType() const {
	return 1;
}

/*
 * to get size of this IteratorSeq
 */
template <class T> size_t VectorIteratorSeq<T>::size() const {
	return v.size();
}

/*
 * to get the element at the given index
 */
template <class T> T VectorIteratorSeq<T>::at(size_t index) const {
	return v.at(index);
}

/*
 * to get all elements
 */
template <class T> vector<T> VectorIteratorSeq<T>::getVector() const {
	return v;
}

/*
 * to reduce the IteratorSeq's elements by a reducing function
 */
template <class T> vector<T> VectorIteratorSeq<T>::reduceLeft(T (*g)(T&, T&)) {
	vector<T> ret;

	if (v.size() > 0) {
		if (v.size() == 1) ret.push_back(v[0]);
		else {
			T t = g(v[0], v[1]);
			for(unsigned int i = 2; i < v.size(); i++) {
				t = g(t, v[i]);
			}
			ret.push_back(t);
		}
	}

	return ret;
}


#endif /* INCLUDE_VECTORITERATORSEQ_HPP_ */
