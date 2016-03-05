/*
 * CollectTask.h
 *
 * RDD::collect() will run CollectTasks
 *
 */

#ifndef COLLECT_H_
#define COLLECT_H_

#include "RDDTask.h"

#include <vector>
using namespace std;

template <class T>
class CollectTask : public RDDTask< T, vector<T> >
{
public:
	    CollectTask(RDD<T> &r, Partition &p);
		vector<T>& run();
		string serialize(vector<T> &t);
		vector<T>& deserialize(string s);

private:
};

#endif
