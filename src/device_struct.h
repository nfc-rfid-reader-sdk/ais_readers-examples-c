/*
 * device_struct.h
 *
 *  Created on: May 10, 2016
 *      Author: SrkoS
 */
#ifndef DEVICE_STRUCT_H_
#define DEVICE_STRUCT_H_

#include <ais_readers.h>

typedef struct S_LOG
{
	int index; //
	int action; //
	int reader_id; //
	int card_id; //
	int system_id; //
	uint8_t nfc_uid[NFC_UID_MAX_LEN]; //
	int nfc_uid_len; // NFC UID length
	uint64_t timestamp; //
} log_t;

typedef struct S_DEVICE
{
	int idx; // index start from 1

	HND_AIS hnd;

	int open;

	DL_STATUS status;
	DL_STATUS status_last; // print

	// event part
	int RealTimeEvents; // indicate new RealTimeEvent(s)
	int LogAvailable; // indicate new data in log buffer
	int LogUnread; //
	int LogUnread_last; //
	// command part
	int cmdResponses; // indicate command finish
	int cmdPercent; // indicate percent of command execution
	// status part
	int DeviceStatus;
	int DeviceStatus_last;
	int TimeoutOccurred; // debug only
	int Status; // additional status

	//--------------------
	c_string SN; // device serial number
	int ID; // device identification number (master)
	//--------------------
	uint32_t relay_state;
	//--------------------
	log_t log;
	//--------------------

	bool cmd_finish; // same as cmdResponses

} device_t, *DEV_HND;

//------------------------------------------------------------------

#endif /* DEVICE_STRUCT_H_ */
