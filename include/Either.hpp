/*
 * Either.hpp
 *
 *  Created on: Feb 26, 2016
 *      Author: yupeng
 */

#ifndef INCLUDE_EITHER_HPP_
#define INCLUDE_EITHER_HPP_

#include "Either.h"

template <class L, class R>
Either<L, R>::Either() {
	type = EITHER_TYPE_NA;
	left = L();
	right = R();
}
template <class L, class R>
void Either<L, R>::initLeft(L &l) {
	left = l;
	type = EITHER_TYPE_LEFT;
}

template <class L, class R>
void Either<L, R>::initRight(R &r) {
	right = r;
	type = EITHER_TYPE_RIGHT;
}

#endif /* INCLUDE_EITHER_HPP_ */
