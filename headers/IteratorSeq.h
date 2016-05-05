/*
 * IteratorSeq.h
 *
 * Just like Iterable in Java.
 * Abstract super class of VectorIteratorSeq & RangeIteratorSeq.
 *
 *  Created on: Jan 28, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_ITERATORSEQ_H_
#define HEADERS_ITERATORSEQ_H_

#include <iostream>
#include <vector>
using std::vector;
using std::istream;
using std::ostream;

#ifndef ITERATORSEQ_DELIMITATION_LEFT
#define ITERATORSEQ_DELIMITATION_LEFT "\a(\a"
#endif
#ifndef ITERATORSEQ_DELIMITATION_RIGHT
#define ITERATORSEQ_DELIMITATION_RIGHT "\a)\a"
#endif

template <class T>
class IteratorSeq {
public:
	virtual ~IteratorSeq();
	virtual int getType() const = 0;
	virtual size_t size() const = 0;
	virtual T at(size_t index) const = 0;
	virtual vector<T> getVector() const = 0;
	virtual vector<T> reduceLeft(T (*g)(T&, T&)) = 0;

	template <class U> IteratorSeq<U> * map(U (*f)(T&));
	template <class U> IteratorSeq<U> * flatMap(vector<U> (*f)(T&));

	bool operator==(const IteratorSeq<T> &s) const;
};

#endif /* HEADERS_ITERATORSEQ_H_ */
