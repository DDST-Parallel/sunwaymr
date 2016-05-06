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

/*
 * constructor
 */
Messaging::Messaging() {
	listenStatus = NA;
	pthread_mutex_init(&mutex_check_file_cache, NULL);
}

/*
 * destructor
 */
Messaging::~Messaging()
{
	// clear cached data
	this->clearAllCache();

}

/*
 * clear all cache, both file cache and shuffle cache
 */
void Messaging::clearAllCache() {
	this->clearFileCache();
	this->clearShuffleCache();
}

/*
 * clear file cache, both cache read by byte and cache read by line
 */
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

/*
 * clear shuffle cache
 */
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

/*
 * read message from a socket file descriptor, save the message to the second parameter ret
 */
void readSocket(int socketfd, string &ret) {
	//int MAX_MESSAGE_SIZE = 1024 * 1024 * 1024; // TODO configuration out of code
	int BUFFER_SIZE = 1024 * 1024 * 4; // receive buffer TODO configuration out of code

	char buffer[BUFFER_SIZE];
	ssize_t total = 0;
	vector<string> strs;
	while (true) {
		memset(buffer, 0, BUFFER_SIZE);
		ssize_t received = recv(socketfd, buffer, BUFFER_SIZE - 1, 0);

		if(received <= 0) break;

		total += received;
		strs.push_back(buffer);
	}

	ret = "";
	for(unsigned int i = 0; i < strs.size(); i++) {
		ret += strs[i];
	}

}

/*
 * send a end flag of messaging, so that the receiver will know the termination of message
 */
bool sendEndOfMessage(int socket_fd) {
	const char* ch = END_OF_MESSAGE;
	int rsend = send(socket_fd, ch, strlen(ch), 0);
	if (rsend < 0)
	{
		Logging::logError("Messaging: sendEndOfMessage: send END_OF_MESSAGE, failed to send");
		return false;
	}
	return true;
}

/*
 * the internal sending function
 */
bool Messaging::sendMessageInternal(int socket_fd, string addr, int targetPort, int msgType, string &msg) {
	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(targetPort);  // set port no.

	inet_pton(AF_INET, addr.c_str(), &address.sin_addr);  // translate IP Address
	int len = sizeof(address);

	// connect to address
	int conn = connect(socket_fd, (struct sockaddr *)&address, len);
	if (conn < 0)
	{
		Logging::logWarning("Messaging: sendMessageInternal: connect fail! will try again");
		while (conn < 0) { // reconnection
			usleep(300000 + rand()%300000); // sleep & reconnect later
			conn = connect(socket_fd, (struct sockaddr *)&address, len);
		}
	}

	// organizing message
	char buffer [33];
	memset(buffer, 0, sizeof(buffer));
	snprintf(buffer, sizeof(buffer), "%d$", msgType);
	string content = buffer;
	content += msg;

	// send data
	const char* ch1 = content.c_str();
	int ret = send(socket_fd, ch1, content.length(), 0);
	if (ret < 0)
	{
		Logging::logError("Messaging: sendMessageInternal: failed to send");
		return false;
	}
	ret = shutdown(socket_fd, SHUT_WR); // shutdown the data transmission
	if (ret < 0)
	{
		Logging::logError("Messaging: sendMessageInternal: failed to shutdown!");
		return false;
	}
	return true;
}

/*
 * send message and wait a reply
 */
bool Messaging::sendMessageForReply(string addr, int targetPort, int msgType, string &msg, string &reply)
{
	int empty_reply_time = 0;
	int empty_reply_time_max = 100;
	bool ret = false;
	while(empty_reply_time < empty_reply_time_max) {
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);

		if (sockfd < 0)
		{
			Logging::logError("Messaging::sendMessageForReply: failed to initialize socket");
			return false;
		}

		if(!sendMessageInternal(sockfd, addr, targetPort, msgType, msg)) {
			return false;
		}

		// read replied data
		readSocket(sockfd, reply);

		if(reply == "") { // failed
			empty_reply_time++;
			// release socket
			close(sockfd);
			usleep(300000 + rand()%300000); // sleep & reconnect later
			continue;
		}
		if(reply == EMPTY_MESSAGE) { // succeeded
			reply = "";
		}
		ret = true;
		// release socket
		close(sockfd);
		break;
	}
	if(!ret) {
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		snprintf(buffer,
				sizeof(buffer),
				"Messaging::sendMessageForReply: failed to send after recv %d empty reply",
				empty_reply_time_max);
		string log = buffer;
		Logging::logError(log);
	}

	return ret;
}

/*
 * send message
 */
bool Messaging::sendMessage(string addr, int targetPort, int msgType, string &msg)
{
	string reply;
	if(!sendMessageForReply(addr, targetPort, msgType, msg, reply)) {
		Logging::logError("Messaging: sendMessage: failed to send");
		return false;
	}

	return true;
}

/*
 * to create a server socket and listen on the port.
 * this function shall be called in another thread if the main thread need do other work
 */
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
	int n = listen(server_sockfd, 36);
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
	while (1) { // always accept
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
		// create a thread to handle socket connection
		pthread_t worker;
		if (pthread_create(&worker, NULL, messageHandler, (void *)td) != 0) {
			Logging::logError("Messaging: failed to create new thread while listening");
			close(client_sockfd);
		}
	}

}

/*
 * get the listening status
 */
int Messaging::getListenStatus() {
	return listenStatus;
}

/*
 * just write the msg to a socket
 */
void send(int socket_fd, string &msg) {
	int rsend = 0;
	if(msg == "") {
		string empty_msg = EMPTY_MESSAGE;
		rsend = send(socket_fd, empty_msg.c_str(), empty_msg.length(), 0);
	}
	else {
		rsend = send(socket_fd, msg.c_str(), msg.length(), 0);
	}
	if (rsend < 0) {
		// send failed
		Logging::logError("Messaging::messageHandler: reply FILE_BLOCK_REQUEST, failed to send");
	}
}

/*
 * thread function for socket connection handling
 */
void* messageHandler(void *data)
{
	// parse data
	struct xyz_messaging_listen_thread_data_ *td = (struct xyz_messaging_listen_thread_data_ *)data;
	Messaging *m = td->mess;

	string msg;
	readSocket(td->client_sockfd, msg); // read data from socket

	string::size_type pos;
	pos=msg.find('$', 0);
	if (pos !=  string::npos) {
		string typeStr = msg.substr(0, pos);
		int msgType = atoi(typeStr.c_str()); // message type
		string msgContent=msg.substr(pos+1);

		if(msgType == FILE_BLOCK_REQUEST) { // the socket is requesting a file block
			vector<string> vs;
			splitString(msgContent, vs, FILE_BLOCK_REQUEST_DELIMITATION);
			if(vs.size() >= 4) {
				string ret;
				string path = vs[0]; // file path
				int offset = atoi(vs[1].c_str()); // offset
				int length = atoi(vs[2].c_str()); // requested length
				FileSourceFormat format = static_cast<FileSourceFormat>(atoi(vs[3].c_str()));
				if (format == FILE_SOURCE_FORMAT_BYTE) { // requesting bytes data
					pthread_mutex_lock(&m->mutex_check_file_cache);
					if (m->file_cache_bytes.find(path) == m->file_cache_bytes.end()) { // check file cache
						string *file_content = new string();
						readFile(path, *file_content); // read file by byte
						m->file_cache_bytes[path] = file_content; // save file content in cache
					} 
					pthread_mutex_unlock(&m->mutex_check_file_cache);

					ret = m->file_cache_bytes[path]->substr(offset, length);
				} else { // requesting lines of content
					pthread_mutex_lock(&m->mutex_check_file_cache);
					if (m->file_cache_lines.find(path) == m->file_cache_lines.end()) { // check file cache
						vector<string> *file_content = new vector<string>();
						readFileToLines(path, *file_content); // read file
						m->file_cache_lines[path] = file_content; // save cache in memory
					} 
					pthread_mutex_unlock(&m->mutex_check_file_cache);

					unsigned end_line = offset+length;
					for (unsigned int i=offset; i<end_line && i<m->file_cache_lines[path]->size(); i++) {
						ret += (*m->file_cache_lines[path])[i];
						ret += "\n";
					}
				}
				send(td->client_sockfd, ret);
				close(td->client_sockfd);
			}
		} else if(msgType == FETCH_REQUEST) {
			vector<string> paras;
			splitString(msgContent, paras, ",");
			if(paras.size() == 2)
			{
				long shuffleID = atol(paras[0].c_str());
				int partitionID = atoi(paras[1].c_str());

				// checking cache
				pthread_mutex_lock(&m->mutex_check_file_cache);
				map< long, vector< vector<string>* > >::iterator it = m->fetch_content_local.find(shuffleID);
				if(it == m->fetch_content_local.end())
				{
					// this shuffle has not  been cached, read it
					// shuffle files directory
					string base_dir = "cache/shuffle/";
					string app_id = num2string(SUNWAYMR_CONTEXT_ID);
				    string shuffle_id = num2string(shuffleID);

				    string dir = app_id.append("/shuffle-") + shuffle_id.append("/");
				    dir = base_dir + dir;
				    vector<string> allFileNames;
					listAllFileNamesContain(dir, allFileNames, "shuffleTaskFile");

					// read file content from every shuffle task file
					vector< vector<string>* > vvs;
					for(unsigned int i=0; i<allFileNames.size(); i++)
					{
							string content;
							readFile(dir+allFileNames[i], content);
							vector<string> *lines = new vector<string>();
							splitString(content, *lines, SHUFFLETASK_PARTITION_DELIMITATION);
							vvs.push_back(lines);
					}
					m->fetch_content_local[shuffleID] = vvs; // save shuffle cache
				}
				pthread_mutex_unlock(&m->mutex_check_file_cache);

				// organize message
				string senMsg;
				for(unsigned int i=0; i<m->fetch_content_local[shuffleID].size()-1; i++)
					senMsg += (*(m->fetch_content_local[shuffleID][i]))[partitionID] + string(SHUFFLETASK_KV_DELIMITATION);
				senMsg += (*(m->fetch_content_local[shuffleID].back()))[partitionID];

				// sending back result
				send(td->client_sockfd, senMsg);
			}
			close(td->client_sockfd);
		} else if(msgType == FILE_INFO || msgType > 999999) { // file transmission of sunwaymrhelper
			m->messageReceived(td->local_port, td->ip, msgType, msgContent);
			string reply = "0";
			send(td->client_sockfd, reply);
			close(td->client_sockfd);
		} else { // messages that will be handled by implementation sub-class of Messaging
			string reply = "";
			send(td->client_sockfd, reply);
			close(td->client_sockfd);

			m->messageReceived(td->local_port, td->ip, msgType, msgContent);
		}
	} else { // invalid socket connection
		string reply = "";
		send(td->client_sockfd, reply);
		close(td->client_sockfd);
	}

	pthread_exit(NULL);
}


#endif /* MESSAGING_HPP_ */
