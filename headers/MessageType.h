/*
 * MessageType.h
 *
 *  Created on: Dec 9, 2015
 *      Author: yupeng
 */

#ifndef MESSAGETYPE_H_
#define MESSAGETYPE_H_

enum MessageType {
	HOST_RESOURCE_INFO, // threads memory listenPort
	FILE_INFO, // appUID fileUID fileName
	SHELL_COMMAND, // command
	A_TASK_RESULT, // jobID taskID valueString
	TASK_RESULT_LIST, // jobID taskID1 valueString1,jobID taskID2 valueString2,jobID taskID3 valueString3
	FILE_BLOCK_REQUEST, // path|offset|length
	FETCH_REQUEST, // shuffleID,partitionID
	RESULT_RENEED, //handle task result lost or rerun
	RESULT_RENEED_TOTAL //obtain job result from master again
};

#endif /* MESSAGETYPE_H_ */
