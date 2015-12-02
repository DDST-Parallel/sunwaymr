/*
 * MappedRDD.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef MAPPEDRDD_H_
#define MAPPEDRDD_H_

#include <vector>
using std::vector;

template <class U, class T>
class MappedRDD : public RDD<U> {
public:
	vector<Partition> getPartitions();
	vector<string> preferredLocations(Partition p);
	vector<U> iteratorArray(Partition p);

private:
	RDD<T> prevRDD;
	U (*mappedFunction)(T);
};



#endif /* MAPPEDRDD_H_ */
