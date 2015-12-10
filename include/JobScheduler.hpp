
#ifndef JOBSCHEDULER_HPP_
#define JOBSCHEDULER_HPP_

#include "JobScheduler.h"

#include <fstream>
#include <iostream>

#include "Scheduler.hpp"
#include "Messaging.hpp"
#include "MessageType.hpp"
#include "Logging.hpp"
#include "Task.hpp"
#include "TaskResult.hpp"
#include "Utils.hpp"

using namespace std;


/* JobScheduler member functions */

JobScheduler::JobScheduler(){
	hostFilePath="";
	master="";
	appName="";
	listenPort=0;
	isMaster=0;
}

JobScheduler::JobScheduler(string hostFP, string mas, string appN, int listenP){
	//initialize
	hostFilePath=hostFP;
	master=mas;
	appName=appN;
	listenPort=listenP;
	isMaster=(mas==getLocalIP()? 1:0);

	//read resource file
	ifstream ins;
	const char * hfp=hostFilePath.c_str();
	ins.open(hfp,ios::in);

	if(!ins){
		logError("fail to open host file");
	}
	string line;

	while(!ins.eof()){
        getline(ins,line);
        vector<string>temp;
        splitString(line,temp," ");
        IPVector.push_back(temp[0]);
        threadCountVector.push_back(atoi(temp[1].c_str()));
        memoryVector.push_back(atoi(temp[2].c_str()));
	}
	ins.close();

	//vector <int> ::iterator s=find(IPVector.begin(),IPVector.end(),getLocalIP());
	selfIPIndex=-1;
	for(unsigned int i=0;i<IPVector.size();i++){
		if (IPVector[i]==getLocalIP()){
			selfIPIndex=i;
			break;
		}
	}


}


bool JobScheduler::start(){



	return true;
}

int JobScheduler::totalThreads(){

	int totalThreadNum=0;

	if (master=="local"){
        //return thread num of local
		string localip=getLocalIP();
		int ipc=IPVector.size();
		for(int i=0;i<ipc;i++){
			if(localip==IPVector[i]){
				totalThreadNum=threadCountVector[i];
				break;
			}
		}
	}
	else{
        //return total thread of all node
		int tc=threadCountVector.size();
		for (int i=0; i<tc;i++){
			totalThreadNum+=threadCountVector[i];
		}
	}
	return totalThreadNum;
}


template <class T>
vector< TaskResult<T>* > JobScheduler::runTasks(vector<Task<T>*> &tasks){

}

void JobScheduler::messageReceived(int localListenPort, string fromHost, int msgType, string msg) {


}

string JobScheduler::getLocalIP() {
	struct ifaddrs * ifAddrStruct = NULL;
	void * tmpAddrPtr = NULL;
	string localIP = "";

	getifaddrs(&ifAddrStruct);

	while (ifAddrStruct != NULL) {
		if (ifAddrStruct->ifa_addr->sa_family == AF_INET) {   // check it is IP4
														  // is a valid IP4 Address
			tmpAddrPtr = &((struct sockaddr_in *) ifAddrStruct->ifa_addr)->sin_addr;
			char addressBuffer[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);

			if (string(ifAddrStruct->ifa_name) == "eth0") {
				localIP = addressBuffer;
			}else{}
		} else {}
		ifAddrStruct = ifAddrStruct->ifa_next;
	}
	return localIP;
}


#endif /* JOBSCHEDULER_HPP_ */
