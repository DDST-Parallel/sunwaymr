/*
 * Scheduler.cpp
 *
 *  Created on: Dec 5, 2015
 *      Author: shi qiuwei
 */

#include "Scheduler.h"
#include "Message.h"
#include "Task.h"
#include "TaskResult.h"
#include "ThreadPool.h"
#include <pthread.h>
#include <fstream>
#include <vector>
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

//#include <sys/types.h>
#define BACKLOG 10  /*maximum connect request*/
#define MAXSIZE 1024
#define SENDPORT 3333

using namespace std;

Scheduler::Scheduler(){
	hostFilePath="";
	master="";
	appName="";
	listenPort=0;
	isMaster=0;
	socketfd=0;
	client_fd=0;
	selfIPRank=0;
}

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


Scheduler::~Scheduler(){}



bool Scheduler::start(){
	return myListen(listenPort);
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

template <class T>
vector< TaskResult<T>* > Scheduler::runTasks(vector<Task<T>*> &tasks){
	int taskNum=tasks.size();
	string selfIP=getLocalIP();
	int taskLauched=0;
	vector< TaskResult<T>* > resultsArray;
	vector< Task<T>* > taskDeal;

	int taskFinished=0;

    //create thread pool with maximum- thread
	ThreadPool<T> tp(taskNum);

	//keep threadRemianVector locally
	for( int i=0;i<taskNum;i++){
		if(tasks[i]->preferredLocations().size()==0){
			continue;
		}
		else{
			preferredIPVector[i]=tasks[i]->preferredLocations()[0];
			int index=vectorFind(IPVector,tasks[i]->preferredLocations()[0]);

			//resources file does not contain target ip-> send message to master to update resource file
			if(index==-1){
                 //re-distribution
				index=vectorIntMax(threadCountVector);
				preferredIPVector[i]=IPVector[index];
			}
			//resources file contains target ip -> keep threadRemainVector to record the thread use situation

				if(threadRemainVector[index]>0){
					threadRemainVector[index]--;
				}
				else{
					//when running, it need waiting.

				}
		}
	}

	//set preferred locations for which is not set before.
	for (int i=0;i<taskNum;i++){
        //no preferred locations -> set it for this task
        if(tasks[i]->preferredLocations().size()==0){
        	int indexNZ=vectorNonZero(threadRemainVector);

        	//no thread remained for this task
        	if(indexNZ==-1){
        		////when running, it need waiting.or send message to master.or record by log.
        		indexNZ=vectorIntMax(threadCountVector);
        		preferredIPVector[i]=IPVector[indexNZ];
        	}
        	//there exists thread resource for this task
        	else{
        		//tasks[i].preferredLocations().push_back(IPVector[indexNZ]);
        		preferredIPVector[i]=IPVector[indexNZ];
        	}
        }
	}

	//add to thread pool
    for (int i=0;i<taskNum;i++){
        //preferredLocation[0] refer to self -> add task to thread pool
        if( preferredIPVector[i]==selfIP){
			tp.addToThreadPool(*tasks[i], i);
			taskDeal.push_back(tasks[i]);
			taskLauched++;

		}
        //preferredLocation[0] refer to others -> do nothing
	    else{}

	}

	Message smessage(getLocalIP(),master,0,"",isMaster);
	if(tp.taskId.size()==tp.taskValue.size()){
		for(unsigned int i=0; i<tp.taskId.size();i++){
			//send out taskId and the corresponding result value
			smessage.taskId=tp.taskId[i];
			std::stringstream ss;
			T& tt=tp.taskValue[i];
			ss << tt;
			smessage.taskValue=ss.str();
			sendMessage(smessage,client_fd);

			//generate taskResultArray
			TaskResult<T>* tr=new TaskResult<T>(*taskDeal[i],tp.taskValue[i]);
			resultsArray.push_back(tr);
			taskFinished++;
		}
	}
	tp.stopAll();
	return resultsArray;
}

bool Scheduler::myListen(int listenPort){
	 /*socketfd for receive message; client_fd for send message*/
     struct sockaddr_in my_addr;
     struct sockaddr_in remote_addr;

     //create socket
     if((socketfd=socket(AF_INET,SOCK_STREAM,0))==-1){
    	 logError("fail to listen: socket create failed");
    	 return false;
     }

     //bind port addr
     my_addr.sin_family=AF_INET;
     my_addr.sin_port=htons(listenPort);
     my_addr.sin_addr.s_addr=INADDR_ANY;
     bzero(&(my_addr.sin_zero),8);
     if (bind(socketfd, (struct sockaddr*)&my_addr, sizeof(struct sockaddr)) == -1) {
    	 logError("fail to listen: bind error!");
     	 return false;
     }

	//listen port
	if (listen(socketfd, BACKLOG) == -1) {
		logError("fail to listen: listen error");
		return false;
	}

	/*client_fd for send message*/
    //create send socket
    if((client_fd=socket(AF_INET,SOCK_STREAM,0))==-1){
    	logError("fail to listen: socket create failed");
    	return false;
    }

    //bind send port addr
    remote_addr.sin_family=AF_INET;
    remote_addr.sin_port=htons(SENDPORT);
    remote_addr.sin_addr.s_addr=INADDR_ANY;
    bzero(&(remote_addr.sin_zero),8);
    if (bind(client_fd, (struct sockaddr*)&remote_addr, sizeof(struct sockaddr)) == -1) {
    	logError("fail to listen: bind error!");
    	return false;
    }

	//listen port
	if (listen(client_fd, BACKLOG) == -1) {
		logError("fail to listen: listen error");
		return false;
	}

	pthread_t * pthread_id1=NULL;
	pthread_create(pthread_id1,NULL,listenReceive,this);

	return true;
}

void* listenReceive(void* data){

	while(1){
		/*receive message from master*/
		Scheduler *s = (Scheduler*)(data);

		Message rMessage;
		rMessage=s->receiveMessage(s->socketfd);

		if(rMessage.isComeFromMaster==1&& s->isMaster==1){ //message:master-> master

		}
		else if(rMessage.isComeFromMaster==0 && s->isMaster==1){//message:slave->master
			//every message contain a taskid and taskvalue
			//master calculate which task is not completed according to global tasks list and calculate the final result
			//maybe master re-distribute tasks to slave
		}
		else if(rMessage.isComeFromMaster==1 && s->isMaster==0){//message:master->slave
			//new information about re-/distribution strategy which need further analyze
			//and then call run tasks

		}else{}
	}
	return (void*)0;
}


Message Scheduler::receiveMessage(int socketfd){
    Message message;

    char buf[1000] = "";
    if(recv(socketfd, buf, sizeof(buf),0)==-1){
   		logError("fail to listen: send error");
   		return message;
   	}

    //deserialize: analyse message information
    vector<string>temp;
    SplitString(buf,temp,",");

    message.myIP=temp[0];
    message.remoteIP=temp[1];
    message.taskId=atoi(temp[2].c_str());
    message.taskValue=temp[3];
    message.isComeFromMaster=atoi(temp[4].c_str());

    return message;
}


bool Scheduler::sendMessage(Message message,int client_fd){
	bool flag=true;

	//serialize
	char icfm[20];
	char trt[20];
	//char trv[20];
	sprintf(trt,"%d",message.taskId);
	//sprintf(trv,"%d",message.taskValue);
	sprintf(icfm,"%d",message.isComeFromMaster);
	string msg=message.myIP.append(",").append(message.remoteIP).append(",").append(trt).append(",").append(message.taskValue).append(",").append(icfm);

	if(send(client_fd, msg.data(), strlen(msg.data()),0)==-1){
		flag=false;
		logError("fail to listen: send error");
	}
	return flag;
}



