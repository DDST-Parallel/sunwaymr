/*
 * SunwayMRPi.cpp
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#include <iostream>
#include "SunwayMRContext.hpp"
#include "ParallelArray.hpp"
#include "MappedRDD.hpp"
using std::cout;
using std::endl;

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


int main(int argc, char *argv[]) {

	cout<< "SunwayMR Pi Calculation" << endl;

	srand(unsigned(time(0)));

	SunwayMRContext sc("SunwayMRPi", argc, argv);

	long times = 1000l;
	ParallelArray<long> p = sc.parallelize(1l, times, 5);
	long d = p.reduce(reduce_f);

	cout << "Array size: " << p.partitions.size() << endl;

	cout << "d: " << d;

	cout << "done." << endl;

	return 0;
}

