/*
 * ShuffledTask.hpp
 *
 *  Created on: 2016年2月22日
 *      Author: knshen
 */

#ifndef INCLUDE_SHUFFLEDTASK_HPP_
#define INCLUDE_SHUFFLEDTASK_HPP_

#include <ShuffledTask.h>
#include "RDD.hpp"
#include "Partition.hpp"
#include "IteratorSeq.hpp"
#include "RDDTask.hpp"
#include "Aggregator.hpp"
#include "HashDivider.hpp"
#include "Utils.hpp"
#include "SunwayMRContext.h"

#include <vector>
#include <string>
#include <fstream>
using namespace std;

template <class T, class U> ShuffledTask<T, U>::ShuffledTask(
		RDD<T> *r, Partition *p, long shID, int nPs,
		HashDivider &hashDivider,
		Aggregator<T, U> &aggregator,
		long (*hFunc)(U &u),
		string (*sf)(U &u))
:RDDTask< T, int >::RDDTask(r, p), hd(hashDivider), agg(aggregator)
{
	shuffleID = shID;
    numPartitions = nPs;
	hashFunc = hFunc;
	strFunc = sf;
}

template <class T, class U> int ShuffledTask<T, U>::run()
{
	// get current RDD value
	IteratorSeq<T> *iter = RDDTask< T, int >::rdd->iteratorSeq(RDDTask< T, int >::partition);
	vector<T> datas = iter->getVector();

	// T -> U
	vector<U> datas1;
	for(unsigned int i=0; i<datas.size(); i++)
		datas1.push_back(agg.createCombiner(datas[i]));

	// get list
    vector< vector<string> > list; // save partitionID -> keys
    for(int i=0; i<numPartitions; i++)
    {
    	vector<string> v;
    	list.push_back(v);
    }

	for(unsigned int i=0; i<datas.size(); i++)
	{
		long hashCode = hashFunc(datas1[i]);
		int pos = hd.getPartition(hashCode); // get the new partition index
		list[pos].push_back(strFunc(datas1[i]));
	}
	//merge data to a string
	string fileContent;
	for(unsigned int i=0; i<list.size(); i++)
	{
		if(list[i].size() == 0)
		{
			fileContent += string(SHUFFLETASK_EMPTY_DELIMITATION) + string(SHUFFLETASK_PARTITION_DELIMITATION);
			continue;
		}
		for(unsigned int j=0; j<list[i].size()-1; j++)
			fileContent += list[i][j] + string(SHUFFLETASK_KV_DELIMITATION);

		fileContent += list[i].back();
		fileContent += string(SHUFFLETASK_PARTITION_DELIMITATION);
	}

	// save to file
	string base_dir = "cache/shuffle/";
	string app_id = num2string(SUNWAYMR_CONTEXT_ID);
	string shuffle_id = num2string(shuffleID);
	string task_id = num2string(this->taskID);

	string dir = app_id.append("/shuffle-") + shuffle_id.append("/");
	dir = base_dir + dir;
	string fileName = "shuffleTaskFile";
	fileName += task_id;
	writeFile(dir, fileName, fileContent);

	return 1;
}



template <class T, class U> string ShuffledTask<T, U>::serialize(int &t)
{
	return to_string(t);
}

template <class T, class U> int ShuffledTask<T, U>::deserialize(string &s)
{
	int val = 0;
	from_string(val, s);
	return val;
}

#endif /* INCLUDE_SHUFFLEDTASK_HPP_ */
