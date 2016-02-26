/*
 * IteratorSeq.hpp
 *
 *  Created on: Dec 3, 2015
 *      Author: yupeng
 */

#ifndef ITERATORSEQ_HPP_
#define ITERATORSEQ_HPP_

#include "IteratorSeq.h"

#include <assert.h>
#include <sstream>
#include <string>
#include "AbstractIteratorSeq.hpp"
#include "RangeIteratorSeq.hpp"
#include "VectorIteratorSeq.hpp"
using std::stringstream;
using std::string;

template <class T> IteratorSeq<T>::IteratorSeq()
{
	iterator = NULL;
	type = 0;
}

template <class T> IteratorSeq<T>::IteratorSeq(T start, T end, T step)
{
	iterator = new RangeIteratorSeq<T>(start, end, step);
	type = 0;
}

template <class T> IteratorSeq<T>::IteratorSeq(T start, T end, T step, bool inclusive)
{
	iterator = new RangeIteratorSeq<T>(start, end, step, inclusive);
	type = 0;
}

template <class T> IteratorSeq<T>::IteratorSeq(vector<T> &v)
{
	iterator = new VectorIteratorSeq<T>(v);
	type = 1;
}

template <class T> IteratorSeq<T>::IteratorSeq(RangeIteratorSeq<T> *r) {
	iterator = r;
	type = 0;
}
template <class T> IteratorSeq<T>::IteratorSeq(VectorIteratorSeq<T> *v) {
	iterator = v;
	type = 1;
}

template <class T> void IteratorSeq<T>::init(vector<T> &v)
{
	iterator = new VectorIteratorSeq<T>(v);
	type = 1;
}

template <class T> long IteratorSeq<T>::size() {
	if (iterator == NULL) return 0;
	return iterator->size();
}

template <class T> T IteratorSeq<T>::at(long index) {
	return iterator->at(index);
}

template <class T> vector<T> IteratorSeq<T>::getVector() {
	if (iterator == NULL) return vector<T>();
	return iterator->getVector();
}

template <class T> template <class U> IteratorSeq<U> IteratorSeq<T>::map(U (*f)(T)) {
	return IteratorSeq<U>(iterator->map(f));
}

template <class T> template <class U> IteratorSeq<U> IteratorSeq<T>::flatMap(vector<U> (*f)(T)) {
	return IteratorSeq<U>(iterator->flatMap(f));
}

template <class T> vector<T>& IteratorSeq<T>::reduceLeft(T (*g)(T,T)) {
	return iterator->reduceLeft(g);
}

template <class T>
ostream& operator<< (ostream &out, IteratorSeq<T> &s) {
	for(long i=0; i<s.size(); i++) {
		out << ITERATORSEQ_DELIMITATION_LEFT
				<< s.at(i)
				<< ITERATORSEQ_DELIMITATION_RIGHT;
	}
	return out;
}

template <class T>
istream& operator>> (istream &in, IteratorSeq<T> &s) {
	std::stringstream buffer;
	buffer << in.rdbuf();
	string str(buffer.str());
	vector<string> vs;
	vector<T> values;
	vs = splitStringByDelimitationCouple(s, ITERATORSEQ_DELIMITATION_LEFT, ITERATORSEQ_DELIMITATION_RIGHT);
	for (unsigned int i=0; i<vs.size(); i++) {
		stringstream ss(vs[i]);
		T t;
		ss << t;
		values.push_back(t);
	}
	s.init(values);
	return in;
}

#endif /* ITERATORSEQ_HPP_ */
