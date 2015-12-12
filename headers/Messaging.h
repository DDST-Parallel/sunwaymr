/*
 * Messaging.h
 *
 *  Created on: Dec 8, 2015
 *      Author: yupeng
 */

#ifndef MESSAGING_H_
#define MESSAGING_H_

#include <string>

#include "MessageType.h"
using std::string;

enum ListenStatus {
	NA,
	SUCCESS,
	FAILURE
};

class Messaging;
struct ThreadData {
	Messaging &mess;
	int local_port;
	string ip;
	int msgType;
	string msgContent;

	ThreadData(Messaging &mess, int port, string ip, int msgType, string msgContent)
	: mess(mess), local_port(port), ip(ip), msgType(msgType), msgContent(msgContent) { }
};

void* messageHandler(void *fd);

class Messaging {
public:
	virtual ~Messaging();

	/*
	 send message in a new thread.
	*/
	bool sendMessage(string addr, int targetPort, int msgType, string msg);

	/*
	 listen a port.
	 call messageReceived in new threads when valid messages arrive.
	 return: true or false(port in use).
	*/
	bool listenMessage(int listenPort);
	int getListenStatus();

	virtual void messageReceived(int localListenPort, string fromHost, int msgType, string msg) = 0;

private:
	int listenStatus;

};


#endif /* MESSAGING_H_ */
