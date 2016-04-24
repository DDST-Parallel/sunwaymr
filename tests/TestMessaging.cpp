/*
 * TestMessaging.cpp
 *
 *  Created on: Jan 3, 2016
 *      Author: yupeng
 */

#include "Messaging.hpp"
#include <pthread.h>

volatile int messageReceivedNum = 0;
pthread_mutex_t mutex_num;

class TestMessaging : public Messaging {
public:
	void messageReceived(int localListenPort, string fromHost, int msgType, string &msg) {
		pthread_mutex_lock(&mutex_num);
		messageReceivedNum++;
		cout << messageReceivedNum << " received : " << msg << endl;
		pthread_mutex_unlock(&mutex_num);
	}
};

struct thread_data{
	TestMessaging *helper;
	const char *msg;
	int v;

   thread_data(TestMessaging *h, const char *m, int v)
   : helper(h), msg(m), v(v) { }
};

void *startHelperListening(void *data) {
	struct thread_data *my_data;
	my_data = (struct thread_data *)data;
	my_data->helper->listenMessage(my_data->v);

	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	pthread_mutex_init(&mutex_num, NULL);
	TestMessaging *t = new TestMessaging();
	int listenPort = 32633;
	if(argc > 1) {
		listenPort = atoi(argv[1]);
	}

//	for (int i=1; i<=1000; i++) {
//		stringstream ss;
//		ss << "hello " << i;
//		string s = ss.str();
//		t->sendMessage("192.168.1.165", listenPort, 0, s);
//		cout << i << " sent" << endl;
//	}

	struct thread_data *data = new thread_data (t, "", listenPort);
	pthread_t thread;
	pthread_mutex_init(&t->mutex_listen_status, NULL);
	pthread_mutex_lock(&t->mutex_listen_status);
	int rc = pthread_create(&thread, NULL, startHelperListening, (void *)data);
	if (rc){
		Logging::logError("JobScheduler: failed to create thread to listen");
	}

	pthread_mutex_lock(&t->mutex_listen_status);
	pthread_mutex_unlock(&t->mutex_listen_status);

	if(t->getListenStatus() == SUCCESS) {

//		for (int i=3331001; i<=3332000; i++) {
//			stringstream ss;
//			ss << "hello " << i;
//			string s = ss.str();
//			t->sendMessage("192.168.1.165", listenPort, 0, s);
//			//cout << i << " sent" << endl;
//		}

		void *status;
		pthread_join(thread, &status); // always listen

		Logging::logInfo("SunwayMRHelper: listen thread finished");
	}

	return 0;
}
