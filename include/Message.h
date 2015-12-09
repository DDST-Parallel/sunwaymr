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
using std::string;

class Message{
public:
	string myIP;
	string remoteIP;

	//slave send to master
	int taskId;
    string taskValue;

	//master send to slave
	//none for now

	//mark for whether come from master or not
	int isComeFromMaster;

	//construct
	Message();
	Message(string myIp,string remoteIp, int tid, string tv, int iCFM);

    virtual ~Message();
};


#endif /* MESSAGE_H_ */
