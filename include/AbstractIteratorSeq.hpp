/*
 * AbstractIteratorSeq.hpp
 *
 *  Created on: Jan 28, 2016
 *      Author: yupeng
 */

#ifndef INCLUDE_ABSTRACTITERATORSEQ_HPP_
#define INCLUDE_ABSTRACTITERATORSEQ_HPP_

#include "AbstractIteratorSeq.h"

template <class T>
AbstractIteratorSeq<T>::~AbstractIteratorSeq() {

}

template <class T>
template <class U> vector<U>& AbstractIteratorSeq<T>::map(U (*f)(T)) {
	vector<U> *ret = new vector<U>();

	for(int i = 0; i < size(); i++) {
		T t = this->at(i);
		U u = f(t);
		ret->push_back(u);
	}

	return *ret;
}

template <class T>
template <class U> vector<U>& AbstractIteratorSeq<T>::flatMap(vector<U> (*f)(T)) {
	vector<U> *ret = new vector<U>();

	for(int i = 0; i < size(); i++) {
		T t = this->at(i);
		vector<U> u = f(t);
		ret->reserve(ret->size() + u.size());
		ret->insert(ret->end(), u.begin(), u.end());
	}

	return *ret;
}



#endif /* INCLUDE_ABSTRACTITERATORSEQ_HPP_ */
