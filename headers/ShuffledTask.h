/*
 * ShuffledTask.h
 *
 * ShuffledRDD::shuffle will create and run ShuffleTasks.
 * ShuffledTask is designed to obtain partition values of ShuffledRDD from previous RDD.
 * Values above will be fetched in ShuffledRDD::iteratorSeq
 *
 *  Created on: 2016年2月22日
 *      Author: knshen
 */

#ifndef HEADERS_SHUFFLEDTASK_H_
#define HEADERS_SHUFFLEDTASK_H_

#include "RDDTask.h"
#include "Aggregator.h"
#include "HashDivider.h"
#include "RDD.h"
#include "Partition.h"

#include <iostream>
#include <string>
#include <sstream>
using namespace std;

template <class T, class U>
class ShuffledTask : public RDDTask< T, int > {
public:
	ShuffledTask(RDD<T> *r, Partition *p, long shID, int nPs,
			HashDivider &hashDivider,
			Aggregator<T, U> &aggregator,
			long (*hFunc)(U &u, stringstream &ss),
			string (*sf)(U &u, stringstream &ss));
	int run();
	string serialize(int &t);
	int deserialize(string &s);

private:
	long shuffleID; // == rddID
	int numPartitions;
	HashDivider hd;
	Aggregator<T, U> agg;
	long (*hashFunc)(U &u, stringstream &ss);
	string (*strFunc)(U &u, stringstream &ss);
};

#endif /* HEADERS_SHUFFLEDTASK_H_ */
