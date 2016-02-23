/*
 * ShuffleTask.hpp
 *
 *  Created on: 2016年2月22日
 *      Author: knshen
 */

#ifndef INCLUDE_SHUFFLETASK_HPP_
#define INCLUDE_SHUFFLETASK_HPP_

#include "ShuffleTask.h"
#include "IteratorSeq.hpp"
#include "RDDTask.hpp"
#include "Aggregator.hpp"
#include "HashDivider.hpp"

#include <vector>
#include <string>
#include <strstream>
#include <sstream>
#include <fstream>
using namespace std;

template <class T> ShuffleTask<T>::ShuffleTask(RDD<T> &r, Partition &p, int shID, int nPs, HashDivider &hashDivider, Aggregator &aggregator, long (*hFunc)(T), string (*sf)(T))
:RDDTask< T, int >::RDDTask(r, p)
{
	shuffleID = shID;
    numPartitions = nPs;
	hd = hashDivider;
	agg = aggregator;
	hashFunc = hFunc;
	strFunc = sf;
}

template <class T> int&  ShuffleTask<T>::run()
{
	IteratorSeq<T> iter = RDDTask< T, int >::rdd.iteratorSeq(RDDTask< T, int >::partition);
	vector<T> datas = iter.getVector;

    vector< vector<string> > list; // save partitionID -> keys
    for(int i=0; i<numPartitions; i++)
    {
    	vector<string> v;
    	list.push_back(v);
    }

	for(int i=0; i<datas.size(); i++)
	{
		long hashCode = hashFunc(datas[i]);
		int pos = hd.getPartition(hashCode); // get the new partition index
		list[pos].push_back(strFunc(datas[i]));
	}

	// save to file
	save2File(list);
	return 1;
}

template <class T> bool ShuffleTask<T>::save2File(vector< vector<string> > list)
{
	ofstream ofs;

    strstream ss;
    string str;
    ss<<shuffleID;
    ss>>str;
	string fileName = str + "mapping";
	ofs.open(fileName.data(), ios::app); // add mode

	if(!ofs)
		return false;

	for(int i=0; i<list.size(); i++)
	{
		for(int j=0; j<list[i].size(); j++)
		{
			ofs<<list[i][j]<<" ";
		}
		ofs<<endl;
	}

	ofs.close();
	return true;
}

template <class T> string ShuffleTask<T>::serialize(int &t)
{
	strstream ss;
	string str;
	ss<<t;
	ss>>str;
	return str;
}

template <class T> int& ShuffleTask<T>::deserialize(string s)
{
	int val;
	stringstream ss;
	ss<<s;
	ss>>val;
	return val;
}

#endif /* INCLUDE_SHUFFLETASK_HPP_ */
