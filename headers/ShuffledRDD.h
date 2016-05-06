/*
 * ShuffledRDD.h
 *
 *  Created on: 2016年2月23日
 *      Author: knshen
 */

#ifndef HEADERS_SHUFFLEDRDD_H_
#define HEADERS_SHUFFLEDRDD_H_

#include "Messaging.h"
#include "IteratorSeq.h"
#include "Partition.h"
#include "RDD.h"
#include "Pair.h"
#include "SunwayMRContext.h"
#include "Aggregator.h"
#include "HashDivider.h"
#include "ShuffledPartition.h"

#include <string>
#include <vector>
#include <map>
#include <tr1/unordered_map>
using namespace std;
using std::tr1::unordered_map;

template <class K, class V, class C>

/*
 * ShuffledRDD means partition values of previous RDD will be redistributed in new partitions.
 * PairRDD::combineByKey, PairRDD::reduceByKey, PairRDD::groupByKey will generate ShuffledRDD.
 */
class ShuffledRDD : public RDD< Pair<K, C> >, public Messaging
{
public:
	ShuffledRDD(RDD< Pair<K, V> > *_prevRDD,
			Aggregator< Pair<K, V>, Pair<K, C> > &_agg,
			HashDivider &_hd,
			long (*hf)(Pair<K, C> &p),
			string (*strf)(Pair<K, C> &p),
			Pair<K, C> (*_recoverFunc)(string &s));
	~ShuffledRDD();
	vector<Partition*> getPartitions();
	vector<string> preferredLocations(Partition *p);
	IteratorSeq< Pair<K, C> > * iteratorSeq(Partition *p);
	void shuffle();
	void messageReceived(int localListenPort, string fromHost, int msgType, string &msg);

private:
	RDD< Pair<K, V> > *prevRDD;
	Aggregator< Pair<K, V>, Pair<K, C> > agg;
	HashDivider hd;
	long (*hashFunc)(Pair<K, C> &p); // function to compute hashCode of a pair
    string (*strFunc)(Pair<K, C> &p); // function  to serialize a pair to string (to save to file)
    Pair<K, C> (*recoverFunc)(string &s); // function to deserialize a string to a pair
    long shuffleID;
    bool shuffleFinished;
    map<int, IteratorSeq< Pair<K, C> >* > shuffleCache; // cache for iteratorSeq()

	void merge(vector<string> &replys, unordered_map<K, C> &combiners); // merge fetched combiners
};


#endif /* HEADERS_SHUFFLEDRDD_H_ */
