/*
 * CollectTask.h
 */

#ifndef COLLECT_H_
#define COLLECT_H_

#include "RDDTask.h"

#include <vector>
using namespace std;

/*
 * RDD::collect() creates and runs CollectTasks
 */
template <class T>
class CollectTask : public RDDTask< T, vector<T> >
{
public:
	    CollectTask(RDD<T> *r, Partition *p);
		vector<T> run();
		string serialize(vector<T> &t); // serialize task result of type T
		vector<T> deserialize(string &s); // deserialize task result from string

private:

};

#endif
