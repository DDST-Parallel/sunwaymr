/*
 * VoidPointerContainer.hpp
 *
 *  Created on: Apr 19, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_POINTERCONTAINER_HPP_
#define HEADERS_POINTERCONTAINER_HPP_

#include "PointerContainer.h"

template <class T>
PointerContainer<T>::PointerContainer(T *ptr, bool autoDelete)
: ptr(ptr), autoDelete(autoDelete) {

}

template <class T>
PointerContainer<T>::~PointerContainer() {
	if(autoDelete) {
		delete this->ptr;
	}
}

template <class T>
T * PointerContainer<T>::get() {
	return this->ptr;
}

#endif /* HEADERS_POINTERCONTAINER_HPP_ */
