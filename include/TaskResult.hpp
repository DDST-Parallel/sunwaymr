/*
 * TaskResult.hpp
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */
#ifndef TASKRESULT_HPP_
#define TASKRESULT_HPP_


#include "TaskResult.h"

#include "Task.hpp"

template <class T> TaskResult<T>::TaskResult(Task<T> &t, T &result)
: task(t), value(result) {

}


#endif /* TASKRESULT_HPP_ */
