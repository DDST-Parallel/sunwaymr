/*
 * RangeIteratorSeq.h
 *
 *  Created on: Jan 28, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_RANGEITERATORSEQ_H_
#define HEADERS_RANGEITERATORSEQ_H_

#include <vector>
#include "IteratorSeq.h"
using std::vector;

/*
 * Sub-class of AbstractIteratorSeq.
 * Containing a seq, like 1...1000
 */
template <class T>
class RangeIteratorSeq : public IteratorSeq<T> {
public:
	RangeIteratorSeq(T start, T end, T step);
	RangeIteratorSeq(T start, T end, T step, bool inclusive);
	int getType() const;
	size_t size() const;
	T at(size_t index) const;
	vector<T> getVector() const;
	vector<T> reduceLeft(T (*g)(T&, T&));

private:
	T start;
	T end;
	T step;
	bool inclusive;
};



#endif /* HEADERS_RANGEITERATORSEQ_H_ */
