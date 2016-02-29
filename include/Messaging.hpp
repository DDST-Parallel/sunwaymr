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

#include <iostream>
#include <cstring>
#include <sstream>

#include "MessageType.hpp"
#include "Utils.hpp"
#include "Logging.hpp"
#include "SunwayMRContext.h"
using namespace std;

Messaging::~Messaging()
{
}

bool Messaging::sendMessage(string addr, int targetPort, int msgType, string msg)
{
	// !!! logging removed since invoked in fork !!!

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		//Logging::logError("Messaging: sendMessage: failed to initialize socket");
		return false;
	}

	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(targetPort);  // set port no.

	inet_pton(AF_INET, addr.c_str(), &address.sin_addr);  // translate IP Address
	int len = sizeof(address);

	// connect
	int conn = connect(sockfd, (struct sockaddr *)&address, len);
	if (conn < 0)
	{
		//Logging::logError("Messaging: sendMessage: connect fail!");
		return false;
	}

	// reorganize packet
	string send_data = "";
	stringstream stream;
	stream << msgType;
	send_data += stream.str() + "$" + msg;

	// send data
	const char* ch = send_data.c_str();
	int byte = send(sockfd, ch, strlen(ch), 0);
	if (byte < 0)
	{
		//Logging::logError("Messaging: sendMessage: send fail!");
		return false;
	}

	// release socket
	close(sockfd);
	return true;
}

string readSocket(int socketfd) {
	int MAX_MESSAGE_SIZE = 1024 * 1024 * 100; // TODO configuration out of code
	int BUFFER_SIZE = 1024 * 1024; // TODO configuration out of code

	char buffer[BUFFER_SIZE];
	int total = 0;
	stringstream msgStream;
	memset(buffer, 0, BUFFER_SIZE);
	int received = read(socketfd, buffer, BUFFER_SIZE);
	while (total < MAX_MESSAGE_SIZE && received > 0) {
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

	return msgStream.str();
}

bool Messaging::sendMessageForReply(string addr, int targetPort, int msgType, string msg, string &reply)
{
	// !!! logging removed since invoked in fork !!!

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		//Logging::logError("Messaging: sendMessage: failed to initialize socket");
		return false;
	}

	struct sockaddr_in address;
	bzero(&address, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_port = htons(targetPort);  // set port no.

	inet_pton(AF_INET, addr.c_str(), &address.sin_addr);  // translate IP Address
	int len = sizeof(address);

	// connect
	int conn = connect(sockfd, (struct sockaddr *)&address, len);
	if (conn < 0)
	{
		//Logging::logError("Messaging: sendMessage: connect fail!");
		return false;
	}

	// reorganize packet
	string send_data = "";
	stringstream stream;
	stream << msgType;
	send_data += stream.str() + "$" + msg + END_OF_MESSAGE;

	// send data
	const char* ch = send_data.c_str();
	int byte = send(sockfd, ch, strlen(ch), 0);
	if (byte < 0)
	{
		//Logging::logError("Messaging: sendMessage: send fail!");
		return false;
	}

	// read replied data
	reply = readSocket(sockfd);

	// release socket
	close(sockfd);
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
		if (client_sockfd < 0)
			continue;

		string ip = inet_ntoa(client_address.sin_addr);

		string msg = readSocket(client_sockfd);

		string::size_type pos;
		pos=msg.find('$', 0);
		if (pos !=  string::npos) {
			string typeStr = msg.substr(0, pos);
			int type = atoi(typeStr.c_str());
			string content=msg.substr(pos+1);
			// thread data encapsulation
			struct ThreadData *td = new ThreadData(*this, listenPort, ip, type, content, client_sockfd);

			// create a thread
			pthread_t worker;
			if (pthread_create(&worker, NULL, messageHandler, (void *)td) != 0) {
				Logging::logError("Messaging: failed to create new thread while listening");
				close(client_sockfd);
			}
		} else {
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
	struct ThreadData *td = (struct ThreadData *)data;
	if(td->msgType == FILE_BLOCK_REQUEST) { // reply file block
		vector<string> vs;
		splitString(td->msgContent, vs, FILE_BLOCK_REQUEST_DELIMITATION);
		if(vs.size() >= 4) {
			string ret;
			string path = vs[0];
			int offset = atoi(vs[1].c_str());
			int length = atoi(vs[2].c_str());
			FileSourceFormat format = static_cast<FileSourceFormat>(atoi(vs[3].c_str()));

			if (format == FILE_SOURCE_FORMAT_BYTE) {
				readFile(path, offset, length, ret);
			} else {
				readFileByLineNumber(path, offset, length, ret);
			}
			int byte = send(td->client_sockfd, ret.c_str(), ret.length(), 0);
			if (byte < 0) {
				// send failed
			}
		}
		close(td->client_sockfd);
	} else if(td->msgType == FETCH_REQUEST) {

		//cout << "CONTEXT ID: " << SUNWAYMR_CONTEXT_ID << endl;

		vector<string> paras;
		splitString(td->msgContent, paras, ",");
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
			map< long, vector< vector<string> > > fetch_content_local; // !global shuffle cache data map(fetch_content) will cause segmentation fault!
			map< long, vector< vector<string> > >::iterator it = fetch_content_local.find(shuffleID);

			if(it == fetch_content_local.end())
			{
				// this shuffle has not  been cached, read it
				vector< vector<string> > vv;
				fetch_content_local[shuffleID] = vv;
				for(unsigned int i=0; i<allFileNames.size(); i++)
				{
					string content;
					readFile(dir+allFileNames[i], content);
					vector<string> lines;
					splitString(content, lines, SHUFFLETASK_PARTITION_DELIMITATION);
					fetch_content_local[shuffleID].push_back(lines);
				}
			}
			//organize send message
			string senMsg;
			for(unsigned int i=0; i<fetch_content_local[shuffleID].size()-1; i++)
				senMsg += fetch_content_local[shuffleID][i][partitionID] + string(SHUFFLETASK_KV_DELIMITATION);
			senMsg += (fetch_content_local[shuffleID].back())[partitionID];

			send(td->client_sockfd, senMsg.c_str(), senMsg.length(), 0);
		}

		close(td->client_sockfd);
	} else {
		close(td->client_sockfd); // close socket

		Messaging &m = td->mess;
		m.messageReceived(td->local_port, td->ip, td->msgType, td->msgContent);
	}

	pthread_exit(NULL);
}


#endif /* MESSAGING_HPP_ */
