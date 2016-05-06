/*
 * VoidPointerContainer.h
 *
 *  Created on: Apr 19, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_POINTERCONTAINER_H_
#define HEADERS_POINTERCONTAINER_H_

/*
 * A PointerContainer hold a pointer to anything(eg. data, function)
 */
template <class T>
class PointerContainer{
public:
	PointerContainer(T *ptr, bool autoDelete);
	~PointerContainer();
	T *get(); // get saved pointer

private:
	T *ptr;
	bool autoDelete; // set deleting pointer in deconstructor or not
};



#endif /* HEADERS_POINTERCONTAINER_H_ */
