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

#include <vector>
#include <ctime>
#include <cstdlib>
#include <iostream>
using namespace std;


double random(double start, double end)
{
	return start+(end-start)*rand()/(RAND_MAX + 1.0);
}

long map_f(long i)
{
	double x = random(-1, 1);
	double y = random(-1, 1);
	if(x*x + y*y <= 1)
		return 1L;
	return 0L;
}

long reduce_f(long x, long y)
{
	return x + y;
}

double compute(long count, int slice)
{
	srand(unsigned(time(0)));

	SunwayMRContext sc("hostFilePath", "127.0.0.1", "knshen", 8080);

	ParallelArray<long> pa = sc.parallelize(1L, count, slice);
	MappedRDD<long, long> map_rdd = pa.map(map_f);
	int num = map_rdd.reduce(reduce_f);
	cout<<(4.0 * num / count)<<endl;
}

int main()
{
	compute(100000000, 100);
	return 0;
}
