/*
 * IteratorSeq.hpp
 *
 *  Created on: Jan 28, 2016
 *      Author: yupeng
 */

#ifndef INCLUDE_ITERATORSEQ_HPP_
#define INCLUDE_ITERATORSEQ_HPP_

#include "IteratorSeq.h"
#include "VectorIteratorSeq.hpp"

/*
 * virtual destructor
 */
template <class T>
IteratorSeq<T>::~IteratorSeq() {

}

/*
 * mapping each element in this IteratorSeq to a new IteratorSeq.
 * the type and value of each element are modified by the map function.
 */
template <class T>
template <class U> IteratorSeq<U> * IteratorSeq<T>::map(U (*f)(T&)) {
	VectorIteratorSeq<U> *ret = new VectorIteratorSeq<U>();

	for(size_t i = 0; i < size(); i++) {
		T t = this->at(i);
		U u = f(t);
		ret->push_back(u);
	}

	return ret;
}

/*
 * flat mapping each element in this IteratorSeq to a new IteratorSeq.
 * the type and value of each element are modified by the flat map function.
 */
template <class T>
template <class U> IteratorSeq<U> * IteratorSeq<T>::flatMap(vector<U> (*f)(T&)) {
	VectorIteratorSeq<U> *ret = new VectorIteratorSeq<U>();

	for(size_t i = 0; i < size(); i++) {
		T t = this->at(i);
		vector<U> u = f(t);
		ret->push_back(u);
	}

	return ret;
}

/*
 * determine the equality of two IteratorSeq
 */
template <class T>
bool IteratorSeq<T>::operator==(const IteratorSeq<T> &s) const {
	return (this->getType() == s.getType() // the same type
			&& this->size() == s.size() // the same number of elements
			&& this->getVector() == s.getVector()); // and the same data set
}

#endif /* INCLUDE_ITERATORSEQ_HPP_ */
