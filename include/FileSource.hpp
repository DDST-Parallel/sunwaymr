/*
 * FileSource.hpp
 *
 *  Created on: Jan 17, 2016
 *      Author: yupeng
 */

#ifndef FILESOURCE_HPP_
#define FILESOURCE_HPP_

#include "FileSource.h"

FileSource::FileSource(string source, string path)
: source(source), path(path) {

}

FileSource::FileSource(FileSource &f)
: source(f.source), path(f.path) {

}

#endif /* FILESOURCE_HPP_ */

