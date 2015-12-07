#include "../src/IteratorSeq.cpp"
#include "../src/Partition.cpp"
#include "../src/ParallelArrayPartition.cpp"
#include "../src/SunwayMRContext.cpp"
#include "../src/Scheduler.cpp"
#include "../src/Logging.cpp"
#include "../src/ParallelArray.cpp"
#include "../src/RDD.cpp"
#include "../src/MappedRDD.cpp"
#include "../src/RDDTask.cpp"
#include "../src/ReduceTask.cpp"
#include "../src/TaskResult.cpp"

#include <iostream>
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
	ParallelArray<int> parallel_array1 = sc.parallelize(data1, 10);
	//ParallelArray<int> parallel_array2 = sc.parallelize(data2, 10);
	//ParallelArray<int> parallel_array3 = sc.parallelize(data3, 10);

	//IteratorSeq<int> iter(1, 100, 1);
	//ParallelArray<int> range = sc.parallelize(iter, 10);

	vector<Partition*> p1 = parallel_array1.getPartitions();
	//vector<Partition*> p2 = parallel_array2.getPartitions();
	//vector<Partition*> p3 = parallel_array3.getPartitions();
	//vector<Partition*> p4 = range.getPartitions();

	cout << parallel_array1.parallelArray_id << ":" << endl;

	
	ParallelArrayPartition<int> *pp = dynamic_cast< ParallelArrayPartition<int> * >(p1[0]);
		cout << pp->rddID << "	" << pp->partitionID << "	" << endl;
		vector<int> data = pp->iteratorSeq().getVector();
	for(int i=0; i<data.size(); i++)
	{
		cout<<data[i]<<" ";
	}
	/*
	for (int i = 0; i < p1.size(); i++)
	{
		ParallelArrayPartition<int> *pp = dynamic_cast< ParallelArrayPartition<int> * >(p1[i]);
		cout << pp->rddID << "	" << pp->partitionID << "	" << endl;
		vector<int> data = pp->iteratorSeq().getVector();
		cout << "data: " << data.size() << endl;
		cout << data[0] << " ~  " << data[data.size() - 1] << endl;

	}*/
	cout << "\n------------------------------------------------------------" << endl;
	
	parallel_array1.map(f);
	return 0;
}
