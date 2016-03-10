/*
 * ais_readers_lib_tester.h
 *
 *  Created on: 08.10.2015.
 *      Author: SrkoS
 *
 * ABCDEFGHIJKLMNOPQRSTUVWXYZ
 * -*----*----+-*-**--*--*---
 * -**---***--*-*****-**-***-
 * abcdefghijklmnopqrstuvwxyz
 *
 * 1234567890
 * ****----------------------
 *
 */
#ifndef AIS_READERS_LIB_TESTER_H_
#define AIS_READERS_LIB_TESTER_H_

#include <ais_readers.h>

// define in IDE
//#define DEV_MIN_PRINTS

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

	bool print_percent_hdr;
	int percent_old;

	DL_STATUS status;

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
	int TimeoutOccurred; // debug only
	int Status; // additional status

	//--------------------
	uint32_t relay_state;
	//--------------------
	log_t log;
	//--------------------

} device_t, *DEV_HND;

void get_list_info(void);

void get_info(DEV_HND dev);

void password_change(DEV_HND dev);

void time_get(DEV_HND dev);
void time_set(DEV_HND dev);

void DoCmd(DEV_HND dev);

//--------------------

void wr_status_(DL_STATUS status, const char * pre_msg);
#define wr_status(pre_msg) wr_status_(dev->status,pre_msg)
char * get_string(void);

#endif /* AIS_READERS_LIB_TESTER_H_ */
