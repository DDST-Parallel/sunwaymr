/*
 * RDDTask.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef RDDTASK_H_
#define RDDTASK_H_


template <class T, class U>
class RDDTask: public Task<U> {
public:
	RDDTask(RDD<T> &r, Partition &p);
	virtual ~RDDTask();
	virtual U run();
	virtual vector<string> preferredLocations();
	virtual RDD<T> &rdd;
	virtual Partition &partition;
};


#endif /* RDDTASK_H_ */
