/*
 * Scheduler.hpp
 *
 *  Created on: Dec 5, 2015
 *      Author: shi qiuwei
 */

#ifndef SCHEDULER_HPP_
#define SCHEDULER_HPP_


#include "Scheduler.h"

#include <pthread.h>
#include <fstream>
#include <iostream>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <string>
#include <stdio.h>
#include <cstring>
#include <iomanip>
#include <stdlib.h>
#include <sstream>
#include <algorithm>
#include <math.h>

#include "Messaging.hpp"
#include "MessageType.hpp"
#include "Logging.hpp"
#include "Task.hpp"
#include "TaskResult.hpp"
#include "ThreadPool.hpp"


using namespace std;

int SplitString(const std::string& str, std::vector<std::string>& ret, const std::string& sep)
{
	if (str.empty())
	    {
	        return 0;
	    }

	    string tmp;
	    string::size_type pos_begin = str.find_first_not_of(sep);
	    string::size_type comma_pos = 0;

	    while (pos_begin != string::npos)
	    {
	        comma_pos = str.find(sep, pos_begin);
	        if (comma_pos != string::npos)
	        {
	            tmp = str.substr(pos_begin, comma_pos - pos_begin);
	            pos_begin = comma_pos + sep.length();
	        }
	        else
	        {
	            tmp = str.substr(pos_begin);
	            pos_begin = comma_pos;
	        }

	        if (!tmp.empty())
	        {
	            ret.push_back(tmp);
	            tmp.clear();
	        }
	    }
	    return 0;
}


int vectorFind(vector<string>& v, string value){
	int index=-1;
	int vs=v.size();
	for( int i=0;i<vs;i++){
		if(v[i]==value){
			index=i;
			break;
		}
	}
	return index;
}
vector<int> vectorFindAll(vector<string>& v, string value){
	vector<int>  indexVector;
	//int index=-1;
	int vs=v.size();
	for( int i=0;i<vs;i++){
		if(v[i]==value){
			//ndex=i;
            indexVector.push_back(i);
		}
	}
	if(indexVector.size()==0){
		indexVector.push_back(-1);
	}

	return indexVector;
}
int vectorNonZero(vector<int>& v){
	int index=-1;
	int vs=v.size();
	for( int i=0;i<vs;i++){
		if(v[i]!=0){
			index=i;
			break;
		}
	}
	return index;
}

int vectorIntMax(vector<int>& v){
	int value=*max_element(v.begin(),v.end());
	int index=-1;
	int vs=v.size();
	for( int i=0;i<vs;i++){
		if(v[i]==value){
			index=i;
			break;
		}
	}
	return index;
}

int vectorSum(vector<int>& v){
	int sum=0;
	int vs=v.size();
	for( int i=0;i<vs;i++){
		sum+=v[i];
	}
	return sum;
}

vector<int> vectorTimes(vector<int>& v,float m){
	vector<int> uv;
	int vs=v.size();
	for( int i=0;i<vs;i++){
		uv.push_back(ceil(v[i]*m));
	}
	return uv;
}


/* Scheduler member functions */

Scheduler::Scheduler(){
	hostFilePath="";
	master="";
	appName="";
	listenPort=0;
	isMaster=0;
	selfIPRank=0;
	allTaskResultsReceived = false;
}

Scheduler::Scheduler(string hostFP, string mas, string appN, int listenP){
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
        SplitString(line,temp," ");
        IPVector.push_back(temp[0]);
        threadCountVector.push_back(atoi(temp[1].c_str()));
        memoryVector.push_back(atoi(temp[2].c_str()));
	}
	ins.close();

	threadRemainVector=threadCountVector;

	//vector <int> ::iterator s=find(IPVector.begin(),IPVector.end(),getLocalIP());
	selfIPRank=0;
	for(unsigned int i=0;i<IPVector.size();i++){
		if (IPVector[i]==getLocalIP()){
			selfIPRank=i;
			break;
		}
	}


}


Scheduler::~Scheduler(){

}


bool Scheduler::start(){



	return true;
}

int Scheduler::totalThreads(){

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
vector< TaskResult<T>* > Scheduler::runTasks(vector<Task<T>*> &tasks){
	allTaskResultsReceived = false;

	int taskNum=tasks.size();
	string selfIP=getLocalIP();

	vector<int> needReDistributing(taskNum,1);

	int diff=taskNum-vectorSum(threadCountVector);
	float res=float(taskNum)/vectorSum(threadCountVector);
	if(diff>0){//need enlarge to keep every task has resources
        threadRemainVector=vectorTimes(threadCountVector,res); // TODO need a right one...
	}

	int taskLauched=0;
	vector< TaskResult<T>* > resultsArray;
	vector< Task<T>* > taskDeal;

	int taskFinished=0;

	//keep threadRemianVector and remainWaitNum locally
	for( int i=0;i<taskNum;i++){
		if(tasks[i]->preferredLocations().size()>0){
			int index=-1;

			int j;
            int plSize=tasks[i]->preferredLocations().size();
			for( j=0;j<plSize;j++){
				index=vectorFind(IPVector,tasks[i]->preferredLocations()[i]);

				if(index!=-1 && threadRemainVector[index]>0){
					threadRemainVector[index]--;
					preferredIPVector[i]=IPVector[index];
					needReDistributing[i]=0;
					break;
				}
			}
			if(j<plSize){
				continue;
			}
		}
	}

	//re-distribution:
	//1.preferred localtions do not meet with resources demand
	//2.no preferred locations

	for( int i=0;i<taskNum;i++){
		if(needReDistributing[i]==1){
			int index=vectorNonZero(threadRemainVector);
			threadRemainVector[index]--;
			preferredIPVector[i]=IPVector[index];
		}
	}

    //create thread pool with maximum- thread
	ThreadPool <T> tp(taskNum);

	//add to thread pool
    for (int i=0;i<taskNum;i++){
        //preferredIPVector refer to self -> add task to thread pool
        if( preferredIPVector[i]==selfIP){
			tp.addToThreadPool(*tasks[i], i);
			taskDeal.push_back(tasks[i]);
			taskLauched++;

		}
        //preferredIPVector refer to others -> do nothing
	    //else{}

	}
    vector< TaskResult<T>* > taskResults
    return waitForTaskResults<T>(taskResults);


	return resultsArray;
}

void Scheduler::messageReceived(int localListenPort, string fromHost, int msgType, string msg) {
	switch(msgType) {
	case A_TASK_RESULT:
	{
		if (isMaster == 1) {
			// TODO add task result to taskResults, if not duplicate


			// TODO check if all task results received...


			allTaskResultsReceived = true;

			// TODO send out to other nodes

		}

		break;
	}

	case TASK_RESULT_LIST:
	{
		if (isMaster == 0) {
			// TODO save task results
			// initialize taskResults

			allTaskResultsReceived = true;

		}

		break;
	}

	}

}

template <class T> vector< TaskResult<T>* > Scheduler::waitForTaskResults(vector< TaskResult<T>* > &taskResults) {

	while (!allTaskResultsReceived) {

	}

	return taskResults;

}

string Scheduler::getLocalIP() {
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


#endif /* SCHEDULER_HPP_ */

