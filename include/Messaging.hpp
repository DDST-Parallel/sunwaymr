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
using namespace std;


Messaging::~Messaging()
{
}

/*
examples:
	 send resource info: send(master_addr, port, MessageType.HOST_RESOURCE_INFO, "threads memory listenPort")
	 send file info: send(addr, port, MessageType.FILE_INFO, "file_uid file_save_path")
	 send file: send(addr, port, file_uid, "file_content...")
	 send shell: send(addr, port, MessageType.SHELL_COMMAND, "~/.local/cache/sunwaymr/file_uid/make")
 */
bool Messaging::sendMessage(string addr, int targetPort, int msgType, string msg)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		cout << "socket init fail!" << endl;
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
		cout << "connect fail!" << endl;
		return false;
	}

	// reorganize packet
	string send_data = "";
	if (msgType == HOST_RESOURCE_INFO)
	{
		// slave send resource info to master
		send_data += "1$" + msg;
	}
	else if (msgType == FILE_INFO)
	{
		// master send file info to slave
		send_data += "2$" + msg;
	}
	else if (msgType == SHELL_COMMAND)
	{
		// master send  shell code to slave
		send_data += "3$" + msg;
	}
	else
	{
		// default: master send file to slave
		stringstream stream;
		stream << msgType;
		send_data += stream.str() + "$" + msg;
	}

	// send data
	const char* ch = send_data.c_str();
	int byte = send(sockfd, ch, strlen(ch), 0);
	if (byte < 0)
	{
		cout << "send fail!" << endl;
		return false;
	}

	// release socket
	close(sockfd);
	return true;
}

bool Messaging::listenMessage(int listenPort)
{
	// init socket
	int *client_sockfd;
	int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (server_sockfd < 0)
		return false;

	struct sockaddr_in server_address;
	struct sockaddr_in client_address;

	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr =  htonl(INADDR_ANY);
	server_address.sin_port = htons(listenPort);
	int server_len = sizeof(server_address);

	// bind
	bind(server_sockfd, (struct sockaddr *)&server_address, server_len);
	socklen_t templen = sizeof(struct sockaddr);

	//listen
	int n = listen(server_sockfd, 10);
	if (n < 0)
		return false;

	cout << "waiting for connect......" << endl;
	while (1) {
		pthread_t worker;
		client_sockfd = (int *)malloc(sizeof(int));
		int client_len = sizeof(client_address);
		*client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_address, (socklen_t *)&client_len);
		if (*client_sockfd < 0)
			return false;

		// get peer ip
		socklen_t namelen = sizeof(client_address);
		char *ip;
		if (getpeername(*client_sockfd, (struct sockaddr *)&client_address, &namelen) == 0 )
			ip = inet_ntoa(*(struct in_addr *)&client_address.sin_addr.s_addr);
		else
			return false;

		// thread data encapsulation
		ThreadData td = {
			client_sockfd,
			this,
			listenPort,
			ip
		};

		// create a thread
		if (pthread_create(&worker, NULL, messageHandler, &td) != 0)
			return false;
	}
	// release
	shutdown(*client_sockfd, 2);
	shutdown(server_sockfd, 2);
	return true;
}

// run of the thread
void* messageHandler(void *data)
{
	// parse data
	ThreadData *td = (struct ThreadData *)data;
	int client_sockfd = *(td->fd);
	Messaging *m = td->mess;
	string fromHost_ip(td->src_ip);

	// receive
	char msg[1024 * 1024]; // 1Mb
	int byte = recv(client_sockfd, msg, 1024 * 1024, 0);
	if (byte < 0)
	{
		cout << "receive fail!" << endl;
		pthread_exit(NULL);
		//exit(EXIT_FAILURE);
	}

	char *head = strtok(msg, "$");
	char *content = strtok(NULL, "$"); // assume there is no '$' in file content
	int type = atoi(head);
	string str_content(content);
	//cout << "$type: " << type << endl;
	//cout << "$content: " << str_content << endl;

	if (type == 1)
		m->messageReceived(td->lcoal_port, fromHost_ip, HOST_RESOURCE_INFO, str_content);
	else if (type == 2)
		m->messageReceived(td->lcoal_port, fromHost_ip, FILE_INFO, str_content);
	else if (type == 3)
		m->messageReceived(td->lcoal_port, fromHost_ip, SHELL_COMMAND, str_content);
	else
		m->messageReceived(td->lcoal_port, fromHost_ip, type, str_content);


	// release
	//free(data);
	close(client_sockfd);
	pthread_exit(NULL);
}


#endif /* MESSAGING_HPP_ */
