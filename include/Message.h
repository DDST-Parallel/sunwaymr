/*
 * Message.h
 *
 *  Created on: Dec 6, 2015
 *      Author: shi qiuwei
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

/*
 * The first version:
 * only consider slave sendMessage
 * and master receiveMessage
 * */

#include <string>
#include <vector>
using std::string;
using std::vector;
#include "TaskResult.h"

template <class T>
class Message{
public:
	string myIP;
	string remoteIP;

	//slave send to master
	//int currentOccupyThreadNum;
	//int currentOccupyMemoryNum;
	vector< TaskResult<T>* > taskResultArray;

	//master send to slave
	//none for now

	//mark for whether come from master or not
	int isComeFromMaster;

	//construct
	Message();
	//Message(string myIp,string remoteIp,int cOTN, int cOMN, int iCFM);
	Message(string myIp,string remoteIp,vector< TaskResult<T>* > taskRA, int iCFM);

    virtual ~Message();
};


#endif /* MESSAGE_H_ */
