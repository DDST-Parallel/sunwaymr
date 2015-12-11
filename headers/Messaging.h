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
	int *fd;
	Messaging *mess;
	int lcoal_port;
	const char *src_ip;
};

void* messageHandler(void *fd);

class Messaging {
public:
	virtual ~Messaging();

	/*
	 send message in a new thread.
	 return: received message, or "" if no message received after some time, or NULL if failed to send.

	 examples:
	 send resource info: send(master_addr, port, MessageType.HOST_RESOURCE_INFO, "threads memory listenPort")
	 send file info: send(addr, port, MessageType.FILE_INFO, "file_uid file_save_path")
	 send file: send(addr, port, file_uid, "file_content...")
	 send shell: send(addr, port, MessageType.SHELL_COMMAND, "~/.local/cache/sunwaymr/file_uid/make")
	*/
	bool sendMessage(string addr, int targetPort, int msgType, string msg);

	/*
	 listen a port in a new thread.
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
