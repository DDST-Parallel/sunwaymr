/*
 * TaskResult.cpp
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#include "TaskResult.h"

template <class T> TaskResult<T>::TaskResult(Task<T> &t, T &result)
: task(t), value(result) {

}
