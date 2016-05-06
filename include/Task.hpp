/*
 * Task.hpp
 *
 *  Created on: Dec 10, 2015
 *      Author: yupeng
 */
#ifndef TASK_HPP_
#define TASK_HPP_

#include "Task.h"

/*
 * constructor
 */
template <class T>
Task<T>::Task()
{
	taskID = XYZ_CURRENT_TASK_ID++;
}
#endif /* TASK_HPP_ */
