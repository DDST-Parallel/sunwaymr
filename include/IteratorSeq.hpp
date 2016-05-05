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

template <class T>
IteratorSeq<T>::~IteratorSeq() {

}

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

template <class T>
template <class U> IteratorSeq<U> * IteratorSeq<T>::flatMap(vector<U> (*f)(T&)) {
	VectorIteratorSeq<U> *ret = new VectorIteratorSeq<U>();

	for(size_t i = 0; i < size(); i++) {
		T t = this->at(i);
		vector<U> u = f(t);
		ret->reserve(ret->size() + u.size());
		ret->push_back(u);
	}

	return ret;
}

//template <class T>
//ostream& operator<< (ostream &out, const IteratorSeq<T> &s) {
//	for(size_t i=0; i<s.size(); i++) {
//		out << ITERATORSEQ_DELIMITATION_LEFT
//				<< s.at(i)
//				<< ITERATORSEQ_DELIMITATION_RIGHT;
//	}
//	return out;
//}

template <class T>
bool IteratorSeq<T>::operator==(const IteratorSeq<T> &s) const {
	return (this->getType() == s.getType()
			&& this->size() == s.size()
			&& this->getVector() == s.getVector());
}

#endif /* INCLUDE_ITERATORSEQ_HPP_ */
