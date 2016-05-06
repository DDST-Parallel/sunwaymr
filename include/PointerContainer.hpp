/*
 * VoidPointerContainer.hpp
 *
 *  Created on: Apr 19, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_POINTERCONTAINER_HPP_
#define HEADERS_POINTERCONTAINER_HPP_

#include "PointerContainer.h"

/*
 * constructor
 */
template <class T>
PointerContainer<T>::PointerContainer(T *ptr, bool autoDelete)
: ptr(ptr), autoDelete(autoDelete) {

}

/*
 * destructor
 */
template <class T>
PointerContainer<T>::~PointerContainer() {
	if(autoDelete) { // delete pointer if autoDelete is set as true
		delete this->ptr;
	}
}

/*
 * to get the original pointer
 */
template <class T>
T * PointerContainer<T>::get() {
	return this->ptr;
}

#endif /* HEADERS_POINTERCONTAINER_HPP_ */
