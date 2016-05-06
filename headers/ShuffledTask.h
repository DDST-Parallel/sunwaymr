/*
 * ShuffledTask.h
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
using namespace std;

/*
 * ShuffledRDD::shuffle will create and run ShuffleTasks.
 * ShuffledTask is designed to obtain partition data of ShuffledRDD from previous RDD.
 * Values above will be fetched in ShuffledRDD::iteratorSeq
 */
template <class T, class U>
class ShuffledTask : public RDDTask< T, int > {
public:
	ShuffledTask(RDD<T> *r, Partition *p, long shID, int nPs,
			HashDivider &hashDivider,
			Aggregator<T, U> &aggregator,
			long (*hFunc)(U &u),
			string (*sf)(U &u));
	int run();
	string serialize(int &t);
	int deserialize(string &s);

private:
	long shuffleID; // the same as rddID
	int numPartitions;
	HashDivider hd;
	Aggregator<T, U> agg;
	long (*hashFunc)(U &u);
	string (*strFunc)(U &u);
};

#endif /* HEADERS_SHUFFLEDTASK_H_ */
