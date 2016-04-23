#ifndef MESSAGING_HPP_
#define MESSAGING_HPP_


#include "Messaging.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>

#include <iostream>
#include <cstring>
#include <sstream>

#include "MessageType.hpp"
#include "Utils.hpp"
#include "Logging.hpp"
#include "SunwayMRContext.h"
using namespace std;


Messaging::Messaging() {
	listenStatus = NA;
	pthread_mutex_init(&mutex_check_file_cache, NULL);
}

Messaging::~Messaging()
{
	// clear cached data
	this->clearAllCache();

}

void Messaging::clearAllCache() {
	this->clearFileCache();
	this->clearShuffleCache();
}

void Messaging::clearFileCache() {
	// file content
	map<string, string* >::iterator it1;
	for(it1=file_cache_bytes.begin(); it1!=file_cache_bytes.end(); ++it1) {
		delete it1->second;
	}
	file_cache_bytes.clear();

	map<string, vector<string>* >::iterator it2;
	for(it2=file_cache_lines.begin(); it2!=file_cache_lines.end(); ++it2) {
		delete it2->second;
	}
	file_cache_lines.clear();
}

void Messaging::clearShuffleCache() {
	// fetch content
	map< long, vector< vector<string>* > >::iterator it3;
	for(it3=fetch_content_local.begin(); it3!=fetch_content_local.end(); ++it3) {
		for (unsigned int i=0; i<it3->second.size(); i++) {
			delete it3->second[i];
		}
		it3->second.clear();
	}
	fetch_content_local.clear();
}

void readSocket(int socketfd, string &ret) {
	//int MAX_MESSAGE_SIZE = 1024 * 1024 * 1024; // TODO configuration out of code
	int BUFFER_SIZE = 1024 * 1024 * 4; // TODO configuration out of code

	char buffer[BUFFER_SIZE];
	ssize_t total = 0;
	stringstream msgStream;
	memset(buffer, 0, BUFFER_SIZE);
	ssize_t received = read(socketfd, buffer, BUFFER_SIZE);
	while (received > 0) { // && total < MAX_MESSAGE_SIZE
		string sb(buffer);
		string::size_type end = sb.find(END_OF_MESSAGE);
		if (end != string::npos) {
			sb = sb.substr(0, end);
			total += end;
			msgStream << sb;
			break;
		}

		total += received;
		msgStream << buffer;

		memset(buffer, 0, BUFFER_SIZE);
		received = read(socketfd, buffer, BUFFER_SIZE);
	}

	ret = msgStream.str();
}

bool Messaging::sendMessageInternal(int socket_fd, string addr, int targetPort, int msgType, string &msg) {
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(targetPort);  // set port no.

	inet_pton(AF_INET, addr.c_str(), &address.sin_addr);  // translate IP Address
	int len = sizeof(address);

	// connect
	int conn = connect(socket_fd, (struct sockaddr *)&address, len);
	if (conn < 0)
	{
		Logging::logWarning("Messaging: sendMessage: connect fail! will try again");
		while (conn < 0) {
			usleep(300000 + rand()%300000); // sleep & reconnect later
			conn = connect(socket_fd, (struct sockaddr *)&address, len);
		}
	}

	// reorganize packet
	string send_data = "";
	stringstream stream;
	stream << msgType;
	send_data += stream.str() + "$";

	// send data
	const char* ch1 = send_data.c_str();
	int ret = send(socket_fd, ch1, send_data.length(), 0);
	if (ret < 0)
	{
		Logging::logError("Messaging: sendMessage: failed to send[1]!");
		return false;
	}
	const char* ch2 = msg.c_str();
	ret = send(socket_fd, ch2, msg.length(), 0);
	if (ret < 0)
	{
		Logging::logError("Messaging: sendMessage: failed to send[2]!");
		return false;
	}
	const char* ch3 = END_OF_MESSAGE;
	ret = send(socket_fd, ch3, strlen(ch3), 0);
	if (ret < 0)
	{
		Logging::logError("Messaging: sendMessage: failed to send[3]!");
		return false;
	}
	return true;
}

bool Messaging::sendMessageForReply(string addr, int targetPort, int msgType, string &msg, string &reply)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		Logging::logError("Messaging: sendMessage: failed to initialize socket");
		return false;
	}

	if(!sendMessageInternal(sockfd, addr, targetPort, msgType, msg)) {
		return false;
	}

	// read replied data
	readSocket(sockfd, reply);

	// release socket
	close(sockfd);
	return true;
}

bool Messaging::sendMessage(string addr, int targetPort, int msgType, string &msg)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		Logging::logError("Messaging: sendMessage: failed to initialize socket");
		return false;
	}

	if(!sendMessageInternal(sockfd, addr, targetPort, msgType, msg)) {
		return false;
	}

	// release socket
	// close(sockfd); // closed by server
	return true;
}

void Messaging::listenMessage(int listenPort)
{
	listenStatus = NA;

	// init socket
	int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sockfd < 0) {
		listenStatus = FAILURE;
		pthread_mutex_unlock(&mutex_listen_status);
		return;
	}

	struct sockaddr_in server_address;

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr =  htonl(INADDR_ANY);
	server_address.sin_port = htons(listenPort);
	int server_len = sizeof(server_address);

	// bind
	int rb = bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
	if (rb < 0)
	{
		listenStatus = FAILURE;
		pthread_mutex_unlock(&mutex_listen_status);
		return;
	}

	//listen
	int n = listen(server_sockfd, 10);
	if (n < 0)
	{
		listenStatus = FAILURE;
		pthread_mutex_unlock(&mutex_listen_status);
		return;
	}

	listenStatus = SUCCESS;
	pthread_mutex_unlock(&mutex_listen_status);

	stringstream listenInfo;
	listenInfo << "Messaging: listening on port [" << listenPort << "]";
	Logging::logInfo(listenInfo.str());

	struct sockaddr_in client_address;
	int client_len = sizeof(sockaddr_in);
	while (1) {
		int client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, (socklen_t *)&client_len);
		if (client_sockfd < 0) {
			stringstream ss;
			ss << "Messaging::listenMessage: accept failed, errno: " << errno;
			Logging::logWarning(ss.str());
			continue;
		}
		string ip = inet_ntoa(client_address.sin_addr);
		// thread data encapsulation
		struct xyz_messaging_listen_thread_data_ *td =
				new xyz_messaging_listen_thread_data_(
						this, listenPort, ip, client_sockfd);
		// create a thread
		pthread_t worker;
		if (pthread_create(&worker, NULL, messageHandler, (void *)td) != 0) {
			Logging::logError("Messaging: failed to create new thread while listening");
			close(client_sockfd);
		}
	}

}

int Messaging::getListenStatus() {
	return listenStatus;
}

// run of the thread
void* messageHandler(void *data)
{
	// parse data
	struct xyz_messaging_listen_thread_data_ *td = (struct xyz_messaging_listen_thread_data_ *)data;
	Messaging *m = td->mess;

	string msg;
	readSocket(td->client_sockfd, msg);

	string::size_type pos;
	pos=msg.find('$', 0);
	if (pos !=  string::npos) {
		string typeStr = msg.substr(0, pos);
		int msgType = atoi(typeStr.c_str());
		string msgContent=msg.substr(pos+1);

		if(msgType == FILE_BLOCK_REQUEST) { // reply file block
			vector<string> vs;
			splitString(msgContent, vs, FILE_BLOCK_REQUEST_DELIMITATION);
			if(vs.size() >= 4) {
				string ret;
				string path = vs[0];
				int offset = atoi(vs[1].c_str());
				int length = atoi(vs[2].c_str());
				FileSourceFormat format = static_cast<FileSourceFormat>(atoi(vs[3].c_str()));
				if (format == FILE_SOURCE_FORMAT_BYTE) {

					pthread_mutex_lock(&m->mutex_check_file_cache);
					if (m->file_cache_bytes.find(path) == m->file_cache_bytes.end()) { // check file cache
						string *file_content = new string();
						readFile(path, *file_content);
						m->file_cache_bytes[path] = file_content;
					} 
					pthread_mutex_unlock(&m->mutex_check_file_cache);

					ret = m->file_cache_bytes[path]->substr(offset, length);
				} else {
					pthread_mutex_lock(&m->mutex_check_file_cache);
					if (m->file_cache_lines.find(path) == m->file_cache_lines.end()) { // check file cache
						vector<string> *file_content = new vector<string>();
						readFileToLines(path, *file_content);
						m->file_cache_lines[path] = file_content;
					} 
					pthread_mutex_unlock(&m->mutex_check_file_cache);

					unsigned end_line = offset+length;
					for (unsigned int i=offset; i<end_line && i<m->file_cache_lines[path]->size(); i++) {
						ret += (*m->file_cache_lines[path])[i];
						ret += "\n";
					}
				}
				int rsend = send(td->client_sockfd, ret.c_str(), ret.length(), 0);
				if (rsend < 0) {
					// send failed
					Logging::logError("Messaging::messageHandler: reply FILE_BLOCK_REQUEST, failed to send[1]");
				}
				const char* ch = END_OF_MESSAGE;
				rsend = send(td->client_sockfd, ch, strlen(ch), 0);
				if (rsend < 0)
				{
					Logging::logError("Messaging: messageHandler: reply FILE_BLOCK_REQUEST, failed to send[2]!");
				}
			}
			//close(td->client_sockfd); // closed by client
		} else if(msgType == FETCH_REQUEST) {

			//cout << "CONTEXT ID: " << SUNWAYMR_CONTEXT_ID << endl;

			vector<string> paras;
			splitString(msgContent, paras, ",");
			if(paras.size() == 2)
			{
				long shuffleID = atol(paras[0].c_str());
				int partitionID = atoi(paras[1].c_str());

				// send back data
				string base_dir = "cache/shuffle/";
				string app_id = num2string(SUNWAYMR_CONTEXT_ID);
			    string shuffle_id = num2string(shuffleID);

			    string dir = app_id.append("/shuffle-") + shuffle_id.append("/");
			    dir = base_dir + dir;
			    vector<string> allFileNames;
				listAllFileNamesContain(dir, allFileNames, "shuffleTaskFile");

				pthread_mutex_lock(&m->mutex_check_file_cache);
				//map< long, vector< vector<string>* > > fetch_content_local; // !global shuffle cache data map(fetch_content) will cause segmentation fault!
				map< long, vector< vector<string>* > >::iterator it = m->fetch_content_local.find(shuffleID);
				if(it == m->fetch_content_local.end())
				{
					// this shuffle has not  been cached, read it
					vector< vector<string>* > vvs;
					for(unsigned int i=0; i<allFileNames.size(); i++)
					{
							string content;
							readFile(dir+allFileNames[i], content);
							vector<string> *lines = new vector<string>();
							splitString(content, *lines, SHUFFLETASK_PARTITION_DELIMITATION);
							vvs.push_back(lines);
					}
					m->fetch_content_local[shuffleID] = vvs;
				}
				pthread_mutex_unlock(&m->mutex_check_file_cache);

				//organize send message
				string senMsg;
				for(unsigned int i=0; i<m->fetch_content_local[shuffleID].size()-1; i++)
					senMsg += (*(m->fetch_content_local[shuffleID][i]))[partitionID] + string(SHUFFLETASK_KV_DELIMITATION);
				senMsg += (*(m->fetch_content_local[shuffleID].back()))[partitionID];

				int rsend = send(td->client_sockfd, senMsg.c_str(), senMsg.length(), 0);
				if (rsend < 0) {
					// send failed
					Logging::logError("Messaging::messageHandler: reply FETCH_REQUEST, failed to send[1]");
				}
				const char* ch = END_OF_MESSAGE;
				rsend = send(td->client_sockfd, ch, strlen(ch), 0);
				if (rsend < 0)
				{
					Logging::logError("Messaging: messageHandler: reply FETCH_REQUEST, failed to send[2]!");
				}
			}

			// close(td->client_sockfd); // closed by client
		} else if(msgType == FILE_INFO || msgType > 999999) { // sunwaymrhelper file sending
			m->messageReceived(td->local_port, td->ip, msgType, msgContent);
			string reply = "0";
			int rsend = send(td->client_sockfd, reply.c_str(), reply.length(), 0);
			if (rsend < 0) {
				// send failed
				Logging::logError("Messaging::messageHandler: reply FILE_INFO/FILE_UID, failed to send[1]");
			}
			const char* ch = END_OF_MESSAGE;
			rsend = send(td->client_sockfd, ch, strlen(ch), 0);
			if (rsend < 0)
			{
				Logging::logError("Messaging: messageHandler: reply FILE_INFO/FILE_UID, failed to send[2]!");
			}
			// close(td->client_sockfd); // closed by client
		} else {
			close(td->client_sockfd); // close socket

			m->messageReceived(td->local_port, td->ip, msgType, msgContent);
		}
	} else {
		close(td->client_sockfd);
	}

	pthread_exit(NULL);
}


#endif /* MESSAGING_HPP_ */
