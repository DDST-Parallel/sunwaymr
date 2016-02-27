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
void Either<L, R>::initLeft(L l) {
	left = l;
	type = EITHER_TYPE_LEFT;
}

template <class L, class R>
void Either<L, R>::initRight(R r) {
	right = r;
	type = EITHER_TYPE_RIGHT;
}

template <class L, class R>
ostream& operator<< (ostream &out, const Either<L, R> &e) {
	if (e.type == EITHER_TYPE_LEFT || e.type == EITHER_TYPE_RIGHT) {
		if (e.type == EITHER_TYPE_LEFT) {
			out << EITHER_DELIMITATION_LEFT
					<< "LEFT"
					<< EITHER_DELIMITATION_RIGHT
					<< EITHER_DELIMITATION_LEFT
					<< e.left
					<< EITHER_DELIMITATION_RIGHT;
		} else if (e.type == EITHER_TYPE_RIGHT) {
			out << EITHER_DELIMITATION_LEFT
					<< "RIGHT"
					<< EITHER_DELIMITATION_RIGHT
					<< EITHER_DELIMITATION_LEFT
					<< e.right
					<< EITHER_DELIMITATION_RIGHT;
		}
	} else {
		out << EITHER_DELIMITATION_LEFT
				<< "NA"
				<< EITHER_DELIMITATION_RIGHT;
	}

	return out;
}

template <class L, class R>
istream& operator>> (istream &in, Either<L, R> &e) {
	std::stringstream buffer;
	buffer << in.rdbuf();
	string s(buffer.str());
	vector<string> vs;
	vs = splitStringByDelimitationCouple(s, EITHER_DELIMITATION_LEFT, EITHER_DELIMITATION_RIGHT);
	if (vs.size() == 2) {
		stringstream ss(vs[1]);
		if (vs[0] == "LEFT") {
			L l;
			ss >> l;
			e.initLeft(l);
		} else if (vs[0] == "RIGHT"){
			R r;
			ss >> r;
			e.initRight(r);
		}
	}
	return in;
}
#endif /* INCLUDE_EITHER_HPP_ */
