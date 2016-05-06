/*
 * VectorIteratorSeq.h
 *
 *  Created on: Jan 28, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_VECTORITERATORSEQ_H_
#define HEADERS_VECTORITERATORSEQ_H_

#include <vector>
#include "IteratorSeq.h"
using std::vector;

/*
 * Sub-class of AbstractIteratorSeq.
 * VectorIteratorSeq is initialized by a vector.
 */
template <class T>
class VectorIteratorSeq : public IteratorSeq<T> {
public:
	VectorIteratorSeq();
	VectorIteratorSeq(vector<T> &v);
	void clear();
	void push_back(T t);
	void push_back(vector<T> &v);
	void reserve(size_t size);
	int getType() const;
	size_t size() const;
	T at(size_t index) const;
	vector<T> getVector() const;
	vector<T> reduceLeft(T (*g)(T&, T&));

private:
	vector<T> v;
};

#endif /* HEADERS_VECTORITERATORSEQ_H_ */
