/*
 * DataCache.h
 *
 *  Created on: May 8, 2016
 *      Author: yupeng
 */

#ifndef HEADERS_DATACACHE_H_
#define HEADERS_DATACACHE_H_

#include <string>
using namespace std;

/*
 * an abstract class.
 * with cache obtaining interfaces(virtual functions).
 */
class DataCache {
public:
	virtual ~DataCache();
	virtual void getData(long dataIndex, string &result) = 0;
};

#endif /* HEADERS_DATACACHE_H_ */
