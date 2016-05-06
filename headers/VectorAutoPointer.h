/*
 * VectorAutoPointer.h
 *
 *  Created on: Apr 18, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_VECTORAUTOPOINTER_H_
#define HEADERS_VECTORAUTOPOINTER_H_

#include <vector>
using std::vector;

/*
 * An auto pointer to hold vector of pointers.
 * In the destructor, the vector of pointers will be deleted.
 */
template <class T>
class VectorAutoPointer {
public:
	VectorAutoPointer(vector<T *> &v);
	~VectorAutoPointer();

private:
	vector<T *> pointers;

};

#endif /* HEADERS_VECTORAUTOPOINTER_H_ */
