/*
 * VectorIteratorSeq.h
 *
 *  Created on: Jan 28, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_VECTORITERATORSEQ_H_
#define HEADERS_VECTORITERATORSEQ_H_

#include <vector>
#include "AbstractIteratorSeq.h"
using std::vector;

template <class T>
class VectorIteratorSeq : public AbstractIteratorSeq<T> {
public:
	VectorIteratorSeq(vector<T> &v);
	long size();
	T at(long index);
	vector<T> getVector();
	vector<T>& reduceLeft(T (*g)(T,T));

private:
	vector<T> v;
};

#endif /* HEADERS_VECTORITERATORSEQ_H_ */
