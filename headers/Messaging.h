/*
 * Messaging.h
 *
 *  Created on: Dec 8, 2015
 *      Author: yupeng
 */

#ifndef MESSAGING_H_
#define MESSAGING_H_

#include <string>
#include <vector>
#include <map>
#include <pthread.h>
using namespace std;

#include "MessageType.h"

#ifndef END_OF_MESSAGE
#define END_OF_MESSAGE "\aEND_OF_MESSAGE\a"
#endif
#ifndef EMPTY_MESSAGE
#define EMPTY_MESSAGE "\aEMPTY_MESSAGE\a"
#endif
#ifndef FILE_BLOCK_REQUEST_DELIMITATION
#define FILE_BLOCK_REQUEST_DELIMITATION "\aFILE_BLOCK_REQUEST\a"
#endif

enum ListenStatus {
	NA,
	SUCCESS,
	FAILURE
};

class Messaging;

/*
 * thread data struct for listen threads
 */
struct xyz_messaging_listen_thread_data_ {
	Messaging *mess;
	int local_port;
	string ip;
	int client_sockfd;

	xyz_messaging_listen_thread_data_(Messaging *mess, int port, string ip, int client_sockfd)
	: mess(mess), local_port(port), ip(ip), client_sockfd(client_sockfd) { }
};


void* messageHandler(void *fd);

/*
 *
 * A useful base class to send and listen socket messages.
 * Messaging is a base class for classes that need networking communication with socket.
 */
class Messaging {
public:
	Messaging();
	virtual ~Messaging();

	void clearAllCache();
	void clearFileCache();
	void clearShuffleCache();

	bool sendMessageForReply(string addr, int targetPort, int msgType, string &msg, string &reply);
	bool sendMessage(string addr, int targetPort, int msgType, string &msg);

	/*
	 listen a port.
	 call messageReceived in new threads when valid messages arrive.
	 return: true or false(port in use).
	*/
	void listenMessage(int listenPort);
	int getListenStatus();

	virtual void messageReceived(int localListenPort, string fromHost, int msgType, string &msg) = 0;

	pthread_mutex_t mutex_listen_status, mutex_check_file_cache;


	map<string, string*> file_cache_bytes;
	map<string, vector<string>*> file_cache_lines;
	/*
	 * shuffleID -> data
	 * data:
	 * <shuffleTask0, shuffleTask1, ...>
	 * shuffleTask0: <partition0, partition1, ...>
	 */
	map< long, vector< vector<string>* > > fetch_content_local;
private:
	int listenStatus;

	bool sendMessageInternal(int socket_fd, string addr, int targetPort, int msgType, string &msg);
};


#endif /* MESSAGING_H_ */
