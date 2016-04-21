
#include <iostream>

#include "IteratorSeq.hpp"
#include "Logging.hpp"
#include "MappedRDD.hpp"
#include "ParallelArray.hpp"
#include "ParallelArrayPartition.hpp"
#include "Partition.hpp"
#include "RDD.hpp"
#include "RDDTask.hpp"
#include "ReduceTask.hpp"
#include "Scheduler.hpp"
#include "SunwayMRContext.hpp"
#include "TaskResult.hpp"
using namespace std;

int f(int i)
{
	if (i % 2 == 0)
		return 1;
	return 0;
}

int g(int i, int j)
{
	return i + j;
}

int main()
{

	vector<int> data1;
	vector<int> data2;
	vector<int> data3;
	for (int i = 1; i < 1001; ++i)
		data1.push_back(i);
	for (int i = 1001; i < 2001; ++i)
		data2.push_back(i);
	for (int i = 2001; i < 3001; ++i)
		data3.push_back(i);


	SunwayMRContext sc("hostFilePath", "127.0.0.1", "knshen", 8080);
	ParallelArrayRDD<int> parallel_array1 = sc.parallelize(data1, 10);
	ParallelArrayRDD<int> parallel_array2 = sc.parallelize(data2, 10);
	ParallelArrayRDD<int> parallel_array3 = sc.parallelize(data3, 10);

	IteratorSeq<int> iter(1, 100, 2);
	ParallelArrayRDD<int> range = sc.parallelize(iter, 10);

	vector<Partition*> p1 = parallel_array1.getPartitions();
	vector<Partition*> p2 = parallel_array2.getPartitions();
	vector<Partition*> p3 = parallel_array3.getPartitions();
	vector<Partition*> p4 = range.getPartitions();

	for (unsigned int i = 0; i < p1.size(); i++)
	{
		ParallelArrayPartition<int> *pp = dynamic_cast< ParallelArrayPartition<int> * >(p1[i]);
		cout << pp->rddID << "	" << pp->partitionID << "	" << endl;
		vector<int> data = pp->iteratorSeq().getVector();
		cout << "data: " << data.size() << endl;
		cout << data[0] << " ~  " << data[data.size() - 1] << endl;

	}
	cout << "\n------------------------------------------------------------" << endl;
	
	for (unsigned int i = 0; i < p2.size(); i++)
	{
		ParallelArrayPartition<int> *pp = dynamic_cast< ParallelArrayPartition<int> * >(p2[i]);
		cout << pp->rddID << "	" << pp->partitionID << "	" << endl;
		vector<int> data = pp->iteratorSeq().getVector();
		cout << "data: " << data.size() << endl;
		cout << data[0] << " ~  " << data[data.size() - 1] << endl;

	}
	cout << "\n------------------------------------------------------------" << endl;

	for (unsigned int i = 0; i < p3.size(); i++)
	{
		ParallelArrayPartition<int> *pp = dynamic_cast< ParallelArrayPartition<int> * >(p3[i]);
		cout << pp->rddID << "	" << pp->partitionID << "	" << endl;
		vector<int> data = pp->iteratorSeq().getVector();
		cout << "data: " << data.size() << endl;
		cout << data[0] << " ~  " << data[data.size() - 1] << endl;

	}
	cout << "\n------------------------------------------------------------" << endl;

	for (unsigned int i = 0; i < p4.size(); i++)
	{
		ParallelArrayPartition<int> *pp = dynamic_cast< ParallelArrayPartition<int> * >(p4[i]);
		cout << pp->rddID << "	" << pp->partitionID << "	" << endl;
		IteratorSeq<int> data = pp->iteratorSeq();
		cout << "data: " << data.size() << endl;
		cout << data.getStart() << " :  " <<data.getStep()<<" : "<< data.getEnd() << endl;

	}
	cout << "\n------------------------------------------------------------" << endl;

	MappedRDD<int, int> map_rdd = parallel_array1.map(f);
	int r = map_rdd.reduce(g);
	cout<<"r: "<<r<<endl;
	return 0;
}
