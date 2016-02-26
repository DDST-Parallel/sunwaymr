/*
 * Task.hpp
 *
 *  Created on: Dec 10, 2015
 *      Author: yupeng
 */
#ifndef TASK_HPP_
#define TASK_HPP_

#include "Task.h"

template <class T> long Task<T>::current_task_id = 1;

template <class T>
Task<T>::Task()
{
	taskID = current_task_id++;
}
#endif /* TASK_HPP_ */
