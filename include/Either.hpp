/*
 * Either.hpp
 *
 *  Created on: Feb 26, 2016
 *      Author: yupeng
 */

#ifndef INCLUDE_EITHER_HPP_
#define INCLUDE_EITHER_HPP_

#include "Either.h"

/*
 * default constructor, the type is not applicable
 */
template <class L, class R>
Either<L, R>::Either() {
	type = EITHER_TYPE_NA;
	left = L();
	right = R();
}

/*
 * initializing the left value
 */
template <class L, class R>
void Either<L, R>::initLeft(L &l) {
	left = l;
	type = EITHER_TYPE_LEFT;
}

/*
 * initializing the right value
 */
template <class L, class R>
void Either<L, R>::initRight(R &r) {
	right = r;
	type = EITHER_TYPE_RIGHT;
}

#endif /* INCLUDE_EITHER_HPP_ */
