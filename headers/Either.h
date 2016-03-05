/*
 * Either.h
 *
 * Containing a value of class L or R.
 *
 *  Created on: Feb 26, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_EITHER_H_
#define HEADERS_EITHER_H_

#include <iostream>
using namespace std;

#ifndef EITHER_DELIMITATION_LEFT
#define EITHER_DELIMITATION_LEFT "\a(\a"
#endif
#ifndef EITHER_DELIMITATION_RIGHT
#define EITHER_DELIMITATION_RIGHT "\a)\a"
#endif

enum EitherType {
	EITHER_TYPE_NA,
	EITHER_TYPE_LEFT,
	EITHER_TYPE_RIGHT
};

template <class L, class R>
class Either {
public:
	Either();
	void initLeft(L l);
	void initRight(R r);
	template <class T, class U>
	friend ostream& operator<< (ostream &out, const Either<T, U> &e);
	template <class T, class U>
	friend istream& operator>> (istream &in, Either<T, U> &e);

	EitherType type;
	L left;
	R right;

};


#endif /* HEADERS_EITHER_H_ */
