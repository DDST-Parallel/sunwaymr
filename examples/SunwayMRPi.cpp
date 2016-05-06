/*
 * SunwayMRPi.cpp
 *
 * Pi computation example of SunwayMR.
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#include <iostream>
#include "SunwayMRContext.hpp"
using std::cout;
using std::endl;

unsigned int seed = time(NULL);

/*
 * generate a random number between start and end
 */
double random(double start, double end)
{
	return start+(end-start)*rand_r(&seed)/(RAND_MAX + 1.0);
}

/*
 * map function for every generated double number
 */
long map_f(long &i)
{
	double x = random(-1, 1);
	double y = random(-1, 1);
	if(x*x + y*y <= 1)
		return 1L;
	return 0L;
}

/*
 * sum the count of points inside the circle
 */
long reduce_f(long &x, long &y)
{
	return x + y;
}

/*
 * main function
 */
int main(int argc, char *argv[]) {
	string start = currentDateTime(); // logging start time of computation
	cout<< "SunwayMR Pi Calculation" << endl;

	srand(unsigned(time(0)));

	SunwayMRContext sc("SunwayMRPi", argc, argv);

	long times = 100000000l;
	int num = sc.parallelize(1l, times)->map(map_f)->reduce(reduce_f);
	double ret = (4.0 * num / times);
	cout << "Pi: " << ret << endl;

	Logging::logInfo(start);
	Logging::logInfo(currentDateTime());
	return 0;
}

