/*
 * Either.h
 *
 *  Created on: Feb 26, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_EITHER_H_
#define HEADERS_EITHER_H_

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

/*
 * Containing a value of class L or R.
 */
template <class L, class R>
class Either {
public:
	Either();
	void initLeft(L &l);
	void initRight(R &r);

	EitherType type; // indicating the value type of stored data
	L left;
	R right;

};


#endif /* HEADERS_EITHER_H_ */
