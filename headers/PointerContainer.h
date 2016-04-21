/*
 * VoidPointerContainer.h
 *
 *  Created on: Apr 19, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_POINTERCONTAINER_H_
#define HEADERS_POINTERCONTAINER_H_

template <class T>
class PointerContainer{
public:
	PointerContainer(T *ptr, bool autoDelete);
	~PointerContainer();
	T *get();

private:
	T *ptr;
	bool autoDelete;
};



#endif /* HEADERS_POINTERCONTAINER_H_ */
