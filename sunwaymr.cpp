/*
 * sunwaymr.cpp
 *
 *  Created on: Dec 11, 2015
 *      Author: yupeng
 */

#ifndef LOGGING_MASK
#define LOGGING_MASK 1
#endif

#include <iostream>
#include <string>
#include <stdlib.h>
#include "SunwayMRHelper.hpp"

using namespace std;

void printUsage() {
	cout << "SunwayMR usage: " << endl;
	cout << "\t[1    listen] sunwaymr -t masterIP masterListenPort threadsNum memoryNum" << endl;
	cout << "\t[2 start app] sunwaymr -a app.cpp [-local]" << endl << endl;
}

int main(int argc, char *argv[]) {
	if (argc <= 1) {
		printUsage();
	} else {
		string cmdType = string(argv[1]);

		if (cmdType == "-t" || cmdType == "-a") {
			if (cmdType == "-t") { // listen
				if (argc >= 6) {
					string master = string(argv[2]);
					int masterListenPort = atoi(argv[3]);
					int threads = atoi(argv[4]);
					int memory = atoi(argv[5]);

					SunwayMRHelper helper = SunwayMRHelper(false);
					helper.start(master, masterListenPort, threads, memory);


				} else {
					printUsage();
				}

			} else { // start app
				if (argc >= 3) {
					string filePath = string(argv[2]);
					bool localMode = false;
					if (argc>3) {
						string argv4 = string(argv[3]);
						if (argv4 == "-local") localMode = true;
					}

					SunwayMRHelper helper = SunwayMRHelper(true);
					helper.runApplication(filePath, localMode);

				} else {
					printUsage();
				}
			}

		} else {
			printUsage();
		}
	}

	return 0;
}


