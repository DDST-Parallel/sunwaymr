/*
 * TextFilePartition.hpp
 *
 *  Created on: Jan 13, 2016
 *      Author: yupeng
 */

#ifndef TEXTFILEPARTITION_HPP_
#define TEXTFILEPARTITION_HPP_

#include "TextFilePartition.h"

#include <iostream>
#include <map>
#include "IteratorSeq.hpp"
#include "Partition.hpp"
#include "TextFileBlock.h"
using namespace std;


TextFilePartition::TextFilePartition(long _rddID, int _partitionID, IteratorSeq<TextFileBlock> &_values)
: rddID(_rddID), partitionID(_partitionID), values(_values)
{
	// map block locations
	map<string, int> locationMap;
	for(int i=0; i<values.size(); i++) {
		TextFileBlock block = values.at(i);
		if(locationMap.find(block.location) != locationMap.end()) {
			locationMap[block.location] = locationMap[block.location] + 1;
		} else {
			locationMap[block.location] = 1;
		}
	}

	// sort
	map<string, int>::iterator it_m;
	for(it_m=locationMap.begin(); it_m!=locationMap.end(); ++it_m) {
		string lo = it_m->first;
		int co = it_m->second;
		bool f = false;

		if(blockLocations.size()==0) {
			blockLocations.push_back(lo);
			f = true;
		}
		vector<string>::iterator it_v;
		for(it_v=blockLocations.begin(); it_v!=blockLocations.end(); ++it_v) {
			if(co>locationMap[*it_v]) {
				blockLocations.insert(it_v, lo);
				f = true;
				break;
			}
		}
		if(!f) {
			blockLocations.push_back(lo);
		}
	}
}

IteratorSeq<TextFileBlock> TextFilePartition::iteratorSeq()
{
	return values;
}

#endif /* TEXTFILEPARTITION_HPP_ */


