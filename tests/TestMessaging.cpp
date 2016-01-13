/*
 * TestMessaging.cpp
 *
 *  Created on: Jan 3, 2016
 *      Author: yupeng
 */

#include "Messaging.hpp"
#include <pthread.h>

int messageReceivedNum = 0;

class TestMessaging : public Messaging {
public:
	void messageReceived(int localListenPort, string fromHost, int msgType, string msg) {
		messageReceivedNum++;
		cout << messageReceivedNum << " received : " << msg << endl;
	}
};

struct thread_data{
	TestMessaging &helper;
	const char *msg;
	int v;

   thread_data(TestMessaging &h, const char *m, int v)
   : helper(h), msg(m), v(v) { }
};

void *startHelperListening(void *data) {
	struct thread_data *my_data;
	my_data = (struct thread_data *)data;
	my_data->helper.listenMessage(my_data->v);

	pthread_exit(NULL);
}

int main() {
	TestMessaging *t = new TestMessaging();
	int listenPort = 32643;

	for (int i=0; i<999; i++) {
		stringstream ss;
		ss << "hello " << i;
		t->sendMessage("192.168.1.63", listenPort, 0, ss.str());
		cout << i << " sent" << endl;
	}

//	struct thread_data *data = new thread_data (*t, "", listenPort);
//	pthread_t thread;
//	pthread_mutex_init(&t->mutex_listen_status, NULL);
//	pthread_mutex_lock(&t->mutex_listen_status);
//	int rc = pthread_create(&thread, NULL, startHelperListening, (void *)data);
//	if (rc){
//		Logging::logError("JobScheduler: failed to create thread to listen");
//	}
//
//	pthread_mutex_lock(&t->mutex_listen_status);
//	pthread_mutex_unlock(&t->mutex_listen_status);
//
//	if(t->getListenStatus() == SUCCESS) {
//
////		for (int i=0; i<999; i++) {
////			stringstream ss;
////			ss << "hello " << i;
////			t->sendMessage("192.168.1.85", listenPort, 0, ss.str());
////			cout << i << " sent" << endl;
////		}
//
//		void *status;
//		pthread_join(thread, &status); // always listen
//
//		Logging::logInfo("SunwayMRHelper: listen thread finished");
//	}

	return 0;
}
