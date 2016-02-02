/*
 * ais_readers.h
 *
 * library version : 4.5.4
 *
 *  Created on: 04.07.2011.
 *      Author: SrkoS
 */

#ifndef AIS_READERS_H_
#define AIS_READERS_H_

#include <stdint.h>
#include <stdbool.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum E_CARD_ACTION
{
	// new card type
	// new card type
//	//-----------------
//	ACTION_REJECTED_BLACKLIST,
//	ACTION_REJECTED_FOREIGN,
//
//	ACTION_UNLOCKED_WHITELIST,
//
//	ACTION_REJECTED_BAD_CRC, // known key
//	ACTION_REJECTED_UNKNOWN_KEY, //
//	ACTION_REJECTED_AUTH, //
//	ACTION_REJECTED_OTHER, // ?
	// new card type
	// new card type
	// new card type


	// CARD_FOREIGN
	// strange card - card from different system
	// BASE> LOG = 0x83 | RTE = 0x00
	ACTION_CARD_FOREIGN = 0x00,

	// DISCARDED
	// blocked card - card on blacklist, no valid access right, has no right of passage
	// BASE> LOG= 0xC3 | RTE= 0x20
	// (32 dec)
	ACTION_CARD_DISCARDED = 0x20,

	// CARD_HACKED
	// Mifare key OK - CRC OK - but bad user data
	// Bad protective data
	// BASE> LOG= 0x84 | RTE= 0x40
	// (64 dec)
	ACTION_CARD_HACKED = 0x40,

	// CARD_BAD_DATA
	// Mifare key OK - CRC BAD
	// Cards with invalid data - BAD CRC
	// BASE> LOG= 0x-- | RTE= 0x82
	// (80 dec)
	ACTION_CARD_BAD_DATA = 0x50,

	// CARD_NO_DATA
	// unreadable card - card without or unknown Mifare key
	// BASE> LOG= 0x-- | RTE= 0x81
	// (96 dec)
	ACTION_CARD_NO_DATA = 0x60,

	// UNLOCKED
	// The correct card
	// BASE> LOG= 0xC2 | RTE= 0x80(+++)
	// TWR> 0x80 (128 dec) - A regular passage (P)
	// TWR> 0x90 (144 dec) - Official exit (S)
	// TWR> 0xA0 (160 dec) - Vehicle pass (V)
	// TWR> 0xB0 (176 dec) - Approved exit (O)
	ACTION_CARD_UNLOCKED = 0x80,
	ACTION_CARD_UNLOCKED_1 = 0x81,
	ACTION_CARD_UNLOCKED_2 = 0x82,
	ACTION_CARD_UNLOCKED_3 = 0x83,
	ACTION_CARD_UNLOCKED_4 = 0x84,
	ACTION_CARD_UNLOCKED_5 = 0x85,
	ACTION_CARD_UNLOCKED_6 = 0x86,
	ACTION_CARD_UNLOCKED_7 = 0x87,

	ACTION_QR_UNLOCKED = 0x70,
	ACTION_QR_BLOCKED = 0x71,
	ACTION_QR_UNKNOWN = 0x72,
// not used anymore
//#define CARD_OK			0x85

// non valid status
// not used status
//	ACTION_DEVICE_MISSING = 0xA1,
//	ACTION_BREAK_THROUGH = 0xA2,
//	ACTION_DOOR_LEFT_OPEN = 0xA3,

	//--------------------
	ACTION_CARD_UNKNOWN	= 0xFF
} e_card_action;
////////////////////////////////////////////////////////////////////////////////////////////////////
//##############################################################################
//--------------------------------------------------------------------------------------------------

#ifdef __cplusplus
#	define EXTC_ extern "C"
#else
#	define EXTC_
#endif

#if __linux__
#	define DL_API EXTC_
#else
// WINDOWS
#	if defined DL_API_STATIC
#		define DL_API EXTC_
#	elif defined DL_API_EXPORTS
#		define DL_API EXTC_ __declspec(dllexport) __stdcall
#	else
#		define DL_API EXTC_ __declspec(dllimport) __stdcall
#	endif
#endif

#if defined(DL_API_EXPORTS)
#	include "handler.h"
#else
	typedef void * HND_AIS;
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "dl_status.h"
#include "ais_readers_list.h"
////////////////////////////////////////////////////////////////////////////////////////////////////
//--------------------------------------------------------------------------------------------------

#define NFC_UID_MAX_LEN			10

////////////////////////////////////////////////////////////////////
/**
 * Type for representing null terminated char array ( aka C-String )
 * Array is always one byte longer ( for null character ) then string
 * Memory space for array must be allocated before use.
 */
typedef const char * c_string;
////////////////////////////////////////////////////////////////////

DL_API
c_string AIS_GetDLLVersion(void);

////////////////////////////////////////////////////////////////////

/**
 * AIS_List_EraseAllDevicesForCheck() : clear list of available devices for checking
 *
 */
DL_API
void AIS_List_EraseAllDevicesForCheck(void);

/**
 * AIS_List_AddDeviceForCheck() : configure DLL - set list of available AIS readers
 *
 * @param device_type int device type by internal specification (enumeration)
 * @param device_id int Reader ID - set by Mifare Init Card
 * @return DL_STATUS
 */
DL_API
DL_STATUS AIS_List_AddDeviceForCheck(int device_type, int device_id);

/**
 * AIS_List_EraseDeviceForCheck() : configure DLL - remove reader from list for checking
 *
 * @param device_type int device type by internal specification (enumeration)
 * @param device_id int Reader ID - set by Mifare Init Card
 * @return DL_STATUS
 */
DL_API
DL_STATUS AIS_List_EraseDeviceForCheck(int device_type, int device_id);

/**
 * Function return which device will be checked.
 *
 * Return pointer to allocated space on heap.
 *
 * @return pair of Device type and ID on the bus delimited with ':'
 * 		Pairs of type:id are delimited with new line character
 */
DL_API
c_string AIS_List_GetDevicesForCheck(void);

/**
 * AIS_List_UpdateAndGetCount()
 *
 * @return number of attached devices <br> <b>negative</b> value if error
 */
DL_API
int AIS_List_UpdateAndGetCount(void);

/**
 *
 * @param pDevice_HND
 * @param pDevice_Serial
 * @param pDevice_Type
 * @param pDevice_ID
 * @param pDevice_FW_VER
 * @param pDevice_CommSpeed
 * @param pDevice_FTDI_Serial
 * @param pDevice_isOpened
 * @param pDevice_Status
 * @param pSystem_Status
 * @return
 */
DL_API
DL_STATUS AIS_List_GetInformation(    //
		HND_AIS *pDevice_HND, //// assigned Handle
		c_string *pDevice_Serial, //// device serial number
		int *pDevice_Type, //// device type - device identification in AIS database
		int *pDevice_ID, //// device identification number (master)
		int *pDevice_FW_VER, //// version of firmware
		int *pDevice_CommSpeed, //// communication speed
		c_string *pDevice_FTDI_Serial, //// FTDI COM port identification
		int *pDevice_isOpened, //// is Device opened
		int *pDevice_Status, //// actual device status
		int *pSystem_Status //// actual system status
		);

///////////////////////////////////////////////////////////////////////////////

/**
 * AIS_List_OpenByHandle
 *
 * @param device : const HND_AIS DeviceHandle
 *
 * @return
 */
DL_API
DL_STATUS AIS_Open(const HND_AIS device);

DL_API
DL_STATUS AIS_Close(HND_AIS device);

// kill object
DL_API
DL_STATUS AIS_Destroy(HND_AIS device);

// global reset service / DLL
DL_API
DL_STATUS AIS_Restart(HND_AIS device);

///////////////////////////////////////////////////////////////////////////////
// XXX :: Info functions

DL_API
DL_STATUS AIS_GetVersion( //
		HND_AIS device, //
		int *hardware_type, //
		// TODO : + hardware version
		int *firmware_version // firmware version
		//		int *ais_type // unit type
		//		int *system_status // system status
		);

///////////////////////////////////////////////////////////////////////////////
// XXX :: Main pump

/**
 *
 *
 * command
 * Use this function for long commands if you want to get percent of execution
 *
 * Example AIS_GetLog_Set() start dumping LOG - function with execution about 10 seconds
 *
 * @param device
 * @param RealTimeEvents
 * @param LogAvailable
 * @param cmdResponses
 * @param cmdPercent
 * @param TimeoutOccurred
 * @param Status
 * @return
 */
DL_API
DL_STATUS AIS_MainLoop(HND_AIS device,
		// event part
		int *RealTimeEvents, // indicate new RealTimeEvent(s)
		int *LogAvailable, // indicate new data in log buffer
		// command part
		int *cmdResponses, // indicate command finish
		int *cmdPercent, // indicate percent of command execution
		// status part
		int *TimeoutOccurred, // debug only
		int *Status // additional status
		);

///////////////////////////////////////////////////////////////////////////////
// XXX :: Time functions

/**
 *
 * @param device
 * @param current_time	GMT timestamp
 * @param timezone		Seconds west of GMT
 * @param DST			is Daylight Saving Time used (If daylight-saving time is ever in use.)
 * @param offset		Seconds west of GMT if Daylight Saving Time used
 * @return
 */
DL_API
DL_STATUS AIS_GetTime(HND_AIS device, uint64_t *current_time, //
		int *timezone, int *DST, int *offset);

/**
 *
 * @param device
 * @param password
 * @param time_to_set	GMT timestamp
 * @param timezone		Seconds west of GMT
 * @param DST			is Daylight Saving Time used (If daylight-saving time is ever in use.)
 * @param offset		Seconds west of GMT if Daylight Saving Time used
 * @return
 */
DL_API
DL_STATUS AIS_SetTime(HND_AIS device, c_string password,
		const uint64_t time_to_set, //
		int timezone, int DST, int offset);

///////////////////////////////////////////////////////////////////////////////

DL_API
DL_STATUS AIS_BatteryGetInfo(HND_AIS device, int *battery_status,
		int *battery_available_percent);

///////////////////////////////////////////////////////////////////////////////
// XXX :: Change password

DL_API
DL_STATUS AIS_ChangePassword(HND_AIS device, c_string old_password,
		c_string new_password);

///////////////////////////////////////////////////////////////////////////////
// XXX :: RTE functions

DL_API
int AIS_ReadRTE_Count(HND_AIS device);

DL_API
DL_STATUS AIS_ReadRTE( //
		HND_AIS device, //
		int * log_index, //
		int * log_action, //
		int * log_reader_id, //
		int * log_card_id, //
		int * log_system_id, //
		uint8_t nfc_uid[NFC_UID_MAX_LEN], //
		int * nfc_uid_len, //
		uint64_t * timestamp //
		);

///////////////////////////////////////////////////////////////////////////////
// XXX :: LOG functions

/**
 * Blocking function
 *
 * @param device
 * @param password
 * @return
 */
DL_API
DL_STATUS AIS_GetLog(HND_AIS device, c_string password);

/**
 * Non-blocking function, must pooling DoCmd
 * get percent of execution
 *
 * Must execute AIS_MainLoop() and wait for command_finish become not null (true)
 *
 * @param device
 * @param password
 * @return
 */
DL_API
DL_STATUS AIS_GetLog_Set(HND_AIS device, c_string password);

/**
 * Non-blocking function, must pooling DoCmd
 * get percent of execution
 *
 * Must execute AIS_MainLoop() and wait for command_finish become not null (true)
 *
 * @param device
 * @param password
 * @param start_index
 * @param end_index
 * @return
 */
DL_API
DL_STATUS AIS_GetLogByIndex(HND_AIS device, c_string password,
		uint32_t start_index, uint32_t end_index);

/**
 * Non-blocking function, must pooling DoCmd
 * get percent of execution
 *
 * Must execute AIS_MainLoop() and wait for command_finish become not null (true)
 *
 * @param device
 * @param password
 * @param time_from
 * @param time_to
 * @return
 */
DL_API
DL_STATUS AIS_GetLogByTime(HND_AIS device, c_string password,
		uint64_t time_from, uint64_t time_to);

// parsed
DL_API
int AIS_ReadLog_Count(HND_AIS device);

/**
 *
 * @param nfc_uid			must provide allocated memory space
 */
DL_API
DL_STATUS AIS_ReadLog( //
		HND_AIS device, //
		int * log_index, //
		int * log_action, //
		int * log_reader_id, //
		int * log_card_id, //
		int * log_system_id, //
		uint8_t nfc_uid[NFC_UID_MAX_LEN], //
		int * nfc_uid_len, //
		uint64_t * timestamp //
		);

DL_API
DL_STATUS AIS_ClearLog(HND_AIS device);

///////////////////////////////////////////////////////////////////////////////
// XXX :: Blacklist functions

DL_API
DL_STATUS AIS_Blacklist_Write(HND_AIS device, c_string password,
		const char str_csv_blacklist[]);

DL_API
DL_STATUS AIS_Blacklist_GetSize(HND_AIS device, c_string password,
		int *size);

DL_API
DL_STATUS AIS_Blacklist_Read(HND_AIS device, char str_csv_blacklist[]);

///////////////////////////////////////////////////////////////////////////////
// XXX :: Whitelist functions

/**
 *
 * @param device : see info about device handle
 * @param password : see info about password
 * @param str_csv_whitelist : eg. "54:A3:34:12, 12.34.56.78, 01234567"
 * 			HEX pairs in UID can be delimited with: ':' or '.' or none
 * 			UID separators: ',' or ';' or other white space
 * 			! NULL or blank string erase white list in device
 * @return
 */
DL_API
DL_STATUS AIS_Whitelist_Write(HND_AIS device, c_string password,
		const char str_csv_whitelist[]);

DL_API
DL_STATUS AIS_Whitelist_Read(HND_AIS device, c_string password,
		c_string *csv_whitelist);

///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
// XRCA Base HD SDK

/**
 * Open Gate with BMR
 * or
 * Open Strike Gate with Base HD
 *
 * @param device
 * @param pulse_duration in milliseconds
 * @return
 */
DL_API
DL_STATUS AIS_LockOpen(HND_AIS device, uint32_t pulse_duration);

DL_API
DL_STATUS AIS_RelayStateSet(HND_AIS device, uint32_t state);

DL_API
DL_STATUS AIS_GetIoState(HND_AIS device, uint32_t *intercom, uint32_t *door,
		uint32_t *relay_state);

/**
 *
 * set value for light: 0 = off, not null = on
 * @param device
 * @param green_master control green light on master unit
 * @param red_master control red light on master unit
 * @param green_slave control green light on slave unit
 * @param red_slave control red light on slave unit
 * @return
 */
DL_API
DL_STATUS AIS_LightControl(HND_AIS device, //
		uint32_t green_master, uint32_t red_master, //
		uint32_t green_slave, uint32_t red_slave);

///////////////////////////////////////////////////////////////////////////////

/**
 * Assign address where FTDI serial ( type: C-string ) are stored
 *
 * @param device
 * @param p_p_ftdi_serial
 * @return
 */
DL_API
DL_STATUS AIS_GetFTDISerial(HND_AIS device, char ** p_p_ftdi_serial);

/**
 * Assign address where FTDI handle ( type: void * ) are stored
 *
 * @param device
 * @param ftdi_handle
 * @return
 */
DL_API
DL_STATUS AIS_GetFTDIHandle(HND_AIS device, void **ftdi_handle);

/**
 * Assign address where FTDI serial ( type: C-string ) are stored,
 * and assign address where FTDI handle ( type: void * ) are stored
 *
 * @param device
 * @param ftdi_serial
 * @param ftdi_handle
 * @return
 */
DL_API
DL_STATUS AIS_GetFTDIInfo(HND_AIS device, char **ftdi_serial, void **ftdi_handle);

///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
//// Helper functions

/*****************************************/
/** Helper function for time-zone manipulation **/

/**
 * return Seconds west of GMT
 */
DL_API
long sys_get_timezone(void);

DL_API
int sys_get_daylight(void);

// _Daylight_savings_bias
DL_API
long sys_get_dstbias(void);

DL_API
c_string sys_get_timezone_info(void);
/*****************************************/

/**
 * ERROR description
 * Get C-string info about status
 *
 * @param status
 * @return
 */
DL_API
c_string dl_status2str(DL_STATUS status);

DL_API
c_string dbg_status2str(DL_STATUS status);

/**
 * Simple helper function
 *
 * Concatenate pre_msg and status string
 *
 * @param status
 * @param pre_msg
 * @return
 */
DL_API
c_string dbg_prn_status(DL_STATUS status, c_string pre_msg);

/**
 * Get card action in C-string representation
 *
 * @param action
 * @return
 */
DL_API
c_string dbg_action2str(e_card_action action);

///////////////////////////////////////////////////////////////////////////////
#if defined(DL_API_EXPORTS)
#	include "ais_readers_undoc.h"
#endif
///////////////////////////////////////////////////////////////////////////////

#endif /* AIS_READERS_H_ */
