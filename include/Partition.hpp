/*
 * Partition.hpp
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef PARTITION_HPP
#define PARTITION_HPP

#include "Partition.h"

#include <vector>
using namespace std;

/*
 * virtual destructor
 */
Partition::~Partition() { }

/*
 * there must be a virtual function for dynamic_cast
 */
void Partition::printInfo() { }

#endif /* PARTITION_HPP */

