/*
 * AbstractIteratorSeq.h
 *
 * Just like Iterable in Java.
 * Abstract super class of VectorIteratorSeq & RangeIteratorSeq.
 * Providing interfaces for IteratorSeq.iterator
 *
 *  Created on: Jan 28, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_ABSTRACTITERATORSEQ_H_
#define HEADERS_ABSTRACTITERATORSEQ_H_

#include <vector>
using std::vector;

template <class T>
class AbstractIteratorSeq {
public:
	virtual ~AbstractIteratorSeq();
	virtual long size() = 0;
	virtual T at(long index) = 0;
	virtual vector<T> getVector() = 0;
	virtual vector<T>& reduceLeft(T (*g)(T,T)) = 0;

	template <class U> vector<U>& map(U (*f)(T));
	template <class U> vector<U>& flatMap(vector<U> (*f)(T));
};



#endif /* HEADERS_ABSTRACTITERATORSEQ_H_ */
