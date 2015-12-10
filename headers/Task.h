/*
 * Task.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef TASK_H_
#define TASK_H_

#include <vector>
#include <string>
using std::vector;
using std::string;


template <class T>
class Task {
public:
	Task(){};
	virtual ~Task(){};
	virtual T& run() = 0;
	virtual string serialize(T &t) = 0;
	virtual T& deserialize(string s) = 0;
	virtual vector<string> preferredLocations() { return vector<string>(0); }
};


#endif /* TASK_H_ */
