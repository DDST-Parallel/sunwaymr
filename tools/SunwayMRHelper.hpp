/*
 * SunwayMRHelper.hpp
 *
 *  Created on: Dec 8, 2015
 *      Author: yupeng
 */

#ifndef SUNWAYMRHELPER_HPP_
#define SUNWAYMRHELPER_HPP_

#include <iostream>
#include <sstream>
#include <cstdlib>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#include "SunwayMRHelper.h"
#include "Utils.hpp"

using namespace std;

struct thread_data{
   SunwayMRHelper *helper;
   const char *msg;
   int v;
};

void *startListening(void *data) {
	struct thread_data *my_data;
	my_data = (struct thread_data *)data;
	my_data->helper->listenMessage(my_data->v);

	pthread_exit(NULL);
}

void *sendHostResourceInfoToMasterRepeatedly(void *data) {
	struct thread_data *my_data;
	my_data = (struct thread_data *)data;

	while(true) {
		sleep(3); // send host resource info every 3 seconds
		my_data->helper->sendHostResourceInfoToMaster(string(my_data->msg));
	}

	pthread_exit(NULL);
}

// for starting user applications
SunwayMRHelper::SunwayMRHelper() {
	masterListenPort = 0;
	threads = 0;
	memory = 0;
	listenPort = 0;
	listening = false;
	sendResourceInfoThread = 0;
}

// for listening
void SunwayMRHelper::start(string masterAddr, int masterListenPort, int threads, int memory) {
	this->masterAddr = masterAddr;
	this->masterListenPort = masterListenPort;
	this->threads = threads;
	this->memory = memory;
	this->localAddr = getLocalHost();

	stringstream ipinfo;
	ipinfo << "SunwayMRHelper: local IP is " << localAddr;
	logInfo(ipinfo.str());

	if (localAddr == "") {
		logError("SunwayMRHelper: failed to obtain local IP address.");
		exit(1);
	}

	logInfo("SunwayMRHelper: starting listening");
	listening = init();
	if (!listening) {
		logError("SunwayMRHelper: failed to start listening.");
		exit(1);
	}
}

SunwayMRHelper::~SunwayMRHelper() {

}

void SunwayMRHelper::setLocalResouce(int threads, int memory) {

	// send repeatedly
	stringstream ss;
	ss << threads << " " << memory << " " << listenPort;
	stringstream info;
	info << "SunwayMRHelper: set local resource: thread: "
			<< threads << " memory: " << memory
			<< " listenPort: " << listenPort;
	logInfo(info.str());

	int v = 0;
	struct thread_data data = {
			this,
			ss.str().c_str(),
			v
	};

	pthread_cancel(sendResourceInfoThread);
	int rc = pthread_create(&sendResourceInfoThread, NULL, sendHostResourceInfoToMasterRepeatedly, (void *)&data);
	if (rc){
		logError("SunwayMRHelper: SunwayMRHelper: failed to create thread to send host resource info.");
		exit(1);
	}
}

void SunwayMRHelper::sendHostResourceInfoToMaster(string msg) {
	stringstream ss;
	ss << "SunwayMRHelper: send resource info to master: " << masterAddr << ":"
			<< masterListenPort << ", message: " << msg;
	logDebug(ss.str());
	Messaging::sendMessage(masterAddr, masterListenPort, HOST_RESOURCE_INFO, msg);
}

void SunwayMRHelper::runApplication(string filePath, bool localMode) {
	// run user application
	string targetHostsFileName = allHostsFileName;
	if (localMode) targetHostsFileName = localHostFileName;

	long appUID = getCurrentTime();
	long fileUID1 = appUID;
	long fileUID2 = appUID + 1;

	string appFileName = fileNameFromPath(filePath);
	string hostsFileName = "hostsFile.lst";

	stringstream filePath2;
	filePath2 << fileSaveDir << targetHostsFileName;

	stringstream fileInfo1, fileInfo2;
	fileInfo1 << fileUID1 << " " << appUID << " " << appFileName;
	fileInfo2 << fileUID2 << " " << appUID << " " << hostsFileName;

	string fileContent1, fileContent2;
	bool rd1 = readFile(filePath, fileContent1);
	if (!rd1) {
		stringstream error1;
		error1 << "SunwayMRHelper: unable to read file: " << filePath;
		logError(error1.str());
	}
	bool rd2 = readFile(filePath2.str(), fileContent2);
	if (!rd2) {
		stringstream error2;
		error2 << "SunwayMRHelper: unable to read file: " << filePath2.str();
		logError(error2.str());
	}


	if (!rd1 || !rd2) {
		exit(1);
	}

	// for all hosts in fileContent2
	stringstream ss(fileContent2);
	string line;
	vector<HostResource> tmp;

	bool sendWithFailure = false;
	while(std::getline(ss,line,'\n')){
		vector<string> v = splitString(line, ' ');
		if (v.size() == 4) {
			string host = v[0];
			int threads = atoi(v[1].c_str());
			int memory = atoi(v[2].c_str());
			int port = atoi(v[3].c_str());
			HostResource hr = {
					host,
					threads,
					memory,
					port
			};
			tmp.push_back(hr);

			// send file info 1
			bool sr = sendMessage(host, port, FILE_INFO, fileInfo1.str());
			if(!sr) {
				sendWithFailure = true;
				stringstream err;
				err << "SunwayMRHelper: failed to send file info[1] to host: " << host << ", " << port << "; info:" << fileInfo1.str();
				logError(err.str());
			}
			// send file info 2
			sr = sendMessage(host, port, FILE_INFO, fileInfo2.str());
			if(!sr) {
				sendWithFailure = true;
				stringstream err;
				err << "SunwayMRHelper: failed to send file info[2] to host: " << host << ", " << port << "; info:" << fileInfo2.str();
				logError(err.str());
			}

			// send file content 1
			sr = sendMessage(host, port, fileUID1, fileContent1);
			if(!sr) {
				sendWithFailure = true;
				stringstream err;
				err << "SunwayMRHelper: failed to send file content[1] to host: " << host << ", " << port;
				logError(err.str());
			}

			// send file content 2
			sr = sendMessage(host, port, fileUID2, fileContent2);
			if(!sr) {
				sendWithFailure = true;
				stringstream err;
				err << "SunwayMRHelper: failed to send file content[2] to host: " << host << ", " << port;
				logError(err.str());
			}
		}
	}

	if(sendWithFailure) exit(1);

	logInfo("SunwayMRHelper: sending files succeeded.");
	logInfo("SunwayMRHelper: starting...");

	string appExecutableName = splitString(appFileName, '.')[0];

	string masterValue = masterAddr;
	if (localMode) masterValue = localAddr;

	int appListenPort = randomValue(30001, 39999);

	stringstream startAppCmd;
	startAppCmd << CXX << " -O2 -g -Wall -fmessage-length=0 "
			<< fileSaveDir << appUID << "/" << appFileName
			<< " -o " << fileSaveDir << appUID << "/" << appExecutableName
			<< " -Itools -Iinclude -Iheaders -pthread -lstdc++ -lm " << endl;
	startAppCmd << fileSaveDir << appUID  << "/" <<  appExecutableName << " " << hostsFileName << " " << masterValue << " " << appListenPort << endl;

	for(unsigned int i=0; i<tmp.size(); i++) {
		sendMessage(tmp[i].host, tmp[i].listenPort, SHELL_COMMAND, startAppCmd.str());
	}

}

bool SunwayMRHelper::initListening(int port) {
	string msg;
	int listenPort = port;
	struct thread_data data = {
			this,
			"",
			listenPort
	};
	pthread_t thread;
	int rc = pthread_create(&thread, NULL, startListening, (void *)&data);
	if (rc){
		logError("JobScheduler: failed to create thread to listen");
	}
	while(getListenStatus() == NA);

	if(getListenStatus() == FAILURE) {
		return false;
	} else {
		setLocalResouce(this->threads, this->memory);
		void *status;
		pthread_join(thread, &status); // always listen

		return true;
	}
}

bool SunwayMRHelper::init() {
	bool ret = false;

	if (localAddr == masterAddr) {
		logInfo("SunwayMRHelper: master is self");
		listenPort = masterListenPort;
		ret = initListening(listenPort);

	} else {
		listenPort = randomValue(20001, 29999);
		ret = initListening(listenPort);

		int try_time = 0;
		while (!ret && try_time < 100) {
			try_time++;
			listenPort += 3;

			ret = initListening(listenPort);
		}
	}

	return ret;
}

void SunwayMRHelper::messageReceived(int localListenPort, string fromHost, int msgType, string msg) {
	if (localListenPort != this->listenPort || fromHost == "" || msg == "") return;

	stringstream received;
	received << "SunwayMRHelper: listening port: " << localListenPort
			<< ", message received from: " << fromHost
			<< ", message type: " << msgType
			<< ", message content: " << msg;
	logDebug(received.str());

	switch(msgType) {
	case HOST_RESOURCE_INFO:
	{
		vector<string> ss = splitString(msg, ' ');
		if (ss.size() == 3) {
			HostResource hr = {
					fromHost,
					atoi(ss[0].c_str()),
					atoi(ss[1].c_str()),
					atoi(ss[2].c_str())
			};
			update(hr);
		}
		break;
	}

	case FILE_INFO:
	{
		vector<string> ss = splitString(msg, ' '); // msg: fileUID, appUID, fileName
		if (ss.size() == 3) {
			int file_uid = atoi(ss[0].c_str());
			fileInfoMap[file_uid] = msg;
		}
		break;
	}

	case SHELL_COMMAND:
	{
		int ret = system(msg.c_str());
		if (ret == -1) {
			stringstream error;
			error << "SunwayMRHelper:: failed to execute shell: " << endl << msg.c_str() << endl;
			logError(error.str());
		}
		break;
	}

	default:
	{
		if(fileInfoMap.find(msgType) != fileInfoMap.end()) {
			string file_info = fileInfoMap[msgType];
			vector<string> vs = splitString(file_info, ' ');
			stringstream sstr;
			sstr << fileSaveDir << vs[1] << "/";
			bool ret = writeFile(sstr.str(), vs[2], msg);
			if (!ret) {
				stringstream error;
				error << "SunwayMRHelper: unable to write file: " << sstr.str() << vs[2];
				logError(error.str());
			}
			fileInfoMap.erase(msgType);
		}

	}
	}
}

void SunwayMRHelper::update(HostResource hr) {
	bool updated = false;
	for (unsigned int i=0; i<allResources.size(); i++) {
		if(allResources[i].host == hr.host) {
			allResources[i].threads = hr.threads;
			allResources[i].memory = hr.memory;
			allResources[i].listenPort = hr.listenPort;
			updated = true;
			break;
		}
	}
	if(!updated) {
		allResources.push_back(hr);
	}

	saveLocalHostFile();
	saveAllHostsFile();
}

void SunwayMRHelper::saveLocalHostFile() {
	stringstream ss;
	ss << localAddr << " " << threads << " " << memory << " " << listenPort;
	bool ret = writeFile(fileSaveDir, localHostFileName, ss.str());
	if (!ret) {
		stringstream error;
		error << "SunwayMRHelper: unable to write file: " << fileSaveDir << localHostFileName;
		logError(error.str());
	}
}

void SunwayMRHelper::saveAllHostsFile() {
	stringstream ss;
	for(unsigned int i=0; i<allResources.size(); i++) {
		ss << allResources[i].host << " " << allResources[i].threads << " " << allResources[i].memory << " " << allResources[i].listenPort << endl;
	}
	bool ret = writeFile(fileSaveDir, allHostsFileName, ss.str());
	if (!ret) {
		stringstream error;
		error << "SunwayMRHelper: unable to write file: " << fileSaveDir << allHostsFileName;
		logError(error.str());
	}
}

#endif /* SUNWAYMRHELPER_HPP_ */
