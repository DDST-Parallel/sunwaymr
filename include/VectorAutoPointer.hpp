/*
 * VectorAutoPointer.hpp
 *
 *  Created on: Apr 18, 2016
 *      Author: yupeng
 */

#ifndef INCLUDE_VECTORAUTOPOINTER_HPP_
#define INCLUDE_VECTORAUTOPOINTER_HPP_

#include "VectorAutoPointer.h"

#include <iostream>
using namespace std;

/*
 * constructor
 */
template <class T>
VectorAutoPointer<T>::VectorAutoPointer(vector<T *> &v)
:pointers(v) {

}

/*
 * destructor, deleting all pointers
 */
template <class T>
VectorAutoPointer<T>::~VectorAutoPointer() {
	for(unsigned int i = 0; i < this->pointers.size(); i++) {
		delete this->pointers[i];
	}
	this->pointers.clear();
}

#endif /* INCLUDE_VECTORAUTOPOINTER_HPP_ */
