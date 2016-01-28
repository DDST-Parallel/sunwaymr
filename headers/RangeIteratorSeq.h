/*
 * RangeIteratorSeq.h
 *
 *  Created on: Jan 28, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_RANGEITERATORSEQ_H_
#define HEADERS_RANGEITERATORSEQ_H_

#include <vector>
#include "AbstractIteratorSeq.h"
using std::vector;

template <class T>
class RangeIteratorSeq : public AbstractIteratorSeq<T> {
public:
	RangeIteratorSeq(T start, T end, T step);
	RangeIteratorSeq(T start, T end, T step, bool inclusive);
	long size();
	T at(long index);
	vector<T> getVector();
	vector<T>& reduceLeft(T (*g)(T,T));

//	T getStart();
//	T getEnd();
//	T getStep();
//	bool isInclusive();
private:
	T start;
	T end;
	T step;
	bool inclusive;
};



#endif /* HEADERS_RANGEITERATORSEQ_H_ */
