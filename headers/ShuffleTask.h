/*
 * ShuffleTask.h
 *
 *  Created on: 2016年2月22日
 *      Author: knshen
 */

#ifndef HEADERS_SHUFFLETASK_H_
#define HEADERS_SHUFFLETASK_H_

#include "RDDTask.h"
#include "Aggregator.h"
#include "HashDivider.h"
#include "RDD.h"
#include "Partition.h"

#include <iostream>
#include <string>
using namespace std;

template <class T, class U>
class ShuffleTask : public RDDTask< T, int > {
public:
	ShuffleTask(RDD<T> &r, Partition &p, long shID, int nPs, HashDivider &hashDivider, Aggregator<T, U> &aggregator, long (*hFunc)(U), string (*sf)(U));
	int& run();
	string serialize(int &t);
	int& deserialize(string s);

private:
	long shuffleID; // == rddID
	int numPartitions;
	HashDivider &hd;
	Aggregator<T, U> &agg;
	long (*hashFunc)(U);
	string (*strFunc)(U);
};

#endif /* HEADERS_SHUFFLETASK_H_ */
