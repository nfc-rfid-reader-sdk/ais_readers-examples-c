/*
 * ais_readers_list.h
 *
 *  Created on: 12.10.2015.
 *      Author: SrkoS
 */
#ifndef AIS_READERS_LIST_H_
#define AIS_READERS_LIST_H_

typedef enum E_KNOWN_DEVICE_TYPES
{
	DL_AIS_100 = 0,
	DL_AIS_20 = 1,
	DL_AIS_30 = 2,
	DL_AIS_35 = 3,
	DL_AIS_50 = 4,
	DL_AIS_110 = 5,
	DL_AIS_LOYALITY = 6,

	// AIS START == BASE BAT USB
	DL_AIS_37 = 7,

	// Half-Duplex // Barcode NFC Reader
	DL_AIS_BMR = 11,

	// Half-Duplex // new base half duplex
	DL_AIS_BASE_HD = 12,

	// Half-Duplex > DL_BASEHD_CONTROL_SDK
	DL_XRCA = 20,

	//--------------------
	DL_AIS_SYSTEM_TYPES_COUNT,
	//--------------------
	DL_UNKNOWN_DEVICE = 0xFF
	//--------------------
} device_e;

#endif /* AIS_READERS_LIST_H_ */
