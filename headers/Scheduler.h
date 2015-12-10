/*
 * Scheduler.h
 *
 *  Created on: Dec 2, 2015
 *      Author: yupeng
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <vector>

#include "Messaging.h"
#include "Logging.h"
#include "Task.h"
#include "TaskResult.h"

using std::vector;


int vectorFind(vector<string>&  v, string value);
vector<int> vectorFindAll(vector<string>& v, string value);
int vectorNonZero(vector<int>& v);
int vectorIntMax(vector<int>& v);
vector<int> vectorTimes(vector<int>& v,float m);
int vectorSum(vector<int>& v);


/*class Scheduler declaration*/
class Scheduler : public Messaging, public Logging {
public:
	Scheduler();
	Scheduler(string hostFilePath, string master, string appName, int listenPort); // master may be "local"
	virtual ~Scheduler();

	virtual bool start(); // may fail to start if listenPort is in use
	virtual int totalThreads();
	template <class T> vector< TaskResult<T>* > runTasks(vector< Task<T>* > &tasks);

	int isMaster;  //whether the node is master or not

private:
	string hostFilePath, master, appName;
	int listenPort;

	int selfIPRank;

	vector<string> IPVector;
	vector<string> preferredIPVector;

	vector<int> threadCountVector;
	vector<int> threadRemainVector;

	vector<int> memoryVector;

	bool allTaskResultsReceived;

	void messageReceived(int localListenPort, string fromHost, int msgType, string msg);
	template <class T> vector< TaskResult<T>* > waitForTaskResults(vector< TaskResult<T>* > &taskResults);

	string getLocalIP();

};

/* class Message */

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
	Message() {
		myIP="";
		remoteIP="";

		taskId=0;
		taskValue="";

		isComeFromMaster=-1;
	}

	Message(string myIp,string remoteIp, int tid, string tv, int iCFM) {
		myIP=myIp;
		remoteIP=remoteIp;
		taskId=tid;
		taskValue=tv;

		isComeFromMaster=iCFM;
	}

    virtual ~Message() {

    }
};


#endif /* SCHEDULER_H_ */
