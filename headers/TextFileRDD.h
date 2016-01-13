/*
 * TextFileRDD.h
 *
 *  Created on: Jan 13, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_TEXTFILERDD_H_
#define HEADERS_TEXTFILERDD_H_

#include <vector>
#include <string>

#include "IteratorSeq.h"
#include "TextFilePartition.h"
#include "RDD.h"
#include "SunwayMRContext.h"
using std::vector;
using std::string;

template <class T> class RDD;
class SunwayMRContext;

class TextFileRDD : public RDD<string> {


};


#endif /* HEADERS_TEXTFILERDD_H_ */
