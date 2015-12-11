/*
 * SunwayMRHelper.h
 *
 *  Created on: Dec 8, 2015
 *      Author: yupeng
 */

#ifndef SUNWAYMRHELPER_H_
#define SUNWAYMRHELPER_H_

#include <vector>
#include <map>

#include "Logging.hpp"
#include "Messaging.hpp"
using std::vector;
using std::map;

struct HostResource {
	string host;
	int threads;
	int memory;
	int listenPort;
};

const string fileSaveDir = "sunwaymrhelper/";
const string localHostFileName = "local.lst";
const string allHostsFileName = "all.lst";
const string CXX = "gcc";

class SunwayMRHelper : public Messaging, public Logging {
public:
	SunwayMRHelper();
	void start(string masterAddr, int masterListenPort, int threads, int memory);
	virtual ~SunwayMRHelper();
	void setLocalResouce(int threads, int memory);
	void sendHostResourceInfoToMaster(string msg);
	void runApplication(string filePath, bool localMode);
	void failedToListen(int port);

private:
	string masterAddr, localAddr;
	int masterListenPort, threads, memory, listenPort;
	vector<HostResource> allResources;
	map<int,string> fileInfoMap;
	bool listening;
	pthread_t sendResourceInfoThread;

	bool init(); // start listening
	bool initListening(int port);
	void messageReceived(int localListenPort, string fromHost, int msgType, string msg);
	void update(HostResource hr);
	void saveLocalHostFile();
	void saveAllHostsFile();

};

#endif /* SUNWAYMRHELPER_H_ */
