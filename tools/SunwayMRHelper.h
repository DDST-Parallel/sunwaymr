/*
 * SunwayMRHelper.h
 *
 *  Created on: Dec 8, 2015
 *      Author: yupeng
 */

#ifndef SRC_SUNWAYMRHELPER_H_
#define SRC_SUNWAYMRHELPER_H_

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

const string fileSaveDir = "~/.cache/sunwaymr/sunwaymrhelper/";
const string localHostFileName = "local.lst";
const string allHostsFileName = "all.lst";
const string CXX = "gcc";

class SunwayMRHelper : public Messaging, public Logging {
public:
	SunwayMRHelper(string masterAddr, int masterListenPort, int threads, int memory);
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

	bool init(); // start listening
	void messageReceived(int localListenPort, string fromHost, int msgType, string msg);
	void update(HostResource hr);
	void saveLocalHostFile();
	void saveAllHostsFile();

};

#endif /* SRC_SUNWAYMRHELPER_H_ */
