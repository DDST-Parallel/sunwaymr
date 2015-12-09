/*
 * Message.cpp
 *
 *  Created on: Dec 6, 2015
 *      Author: shi qiuwei
 */

#include "Message.h"


Message::Message(){
	myIP="";
	remoteIP="";

	taskId=0;
	taskValue="";

	isComeFromMaster=-1;
}

Message::Message(string myIp,string remoteIp,int tid, string tv, int iCFM){
	myIP=myIp;
	remoteIP=remoteIp;
	taskId=tid;
	taskValue=tv;

	isComeFromMaster=iCFM;
}

Message::~Message(){

}
