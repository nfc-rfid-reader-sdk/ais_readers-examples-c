/*
 ============================================================================
 Name        : ais_readers_lib_tester.c
 Author      : Srdjan Kostic
 Version     :
 Copyright   : www.d-logic.net
 Description : Tester for ais_readers library in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MINIMAL_LIB_VERSION			"4.9.1"

#define MENU_COL_WIDTH		30
#define MENU_COL_NUMBER		3

#include "ais_readers_lib_tester.h"
#include "device_list.h"

#define DEFAULT_RTE_TIME			10
//---------------------------------------------------------
char pass[64] = //
//		"55555";
		"1111";
//		"";
char new_pass[64] = //
		"1111";

//------------------------------------------------------------------
static c_string prn_format = "%4d |%34s| %5d | %7d | %5d";
//------------------------------------------------------------------

static c_string hdr[] =
{
  "-----+----------------------------------+-------+---------+-------+--------------------------+-------------------------------------------",
  " Idx |              action              | RD ID | Card ID | JobNr |    NFC [length] : UID    | Time-stamp,      Date - Time" };
//"-----+----------------------------------+-------+---------+-------+--------------------------+-------------------------------------------
//"   0 | [64(0x40):ACTION_CARD_UNLOCKED_5]|    19 |       0 |       | [7]:04:55:19:EA:31:33:80 | GMT= 1456233460, Tue Feb 23 13:17:40 2016
//"   1 |[128(0x80):ACTION_CARD_UNLOCKED_5]|    19 | 1234567 | 12345 | [7]:04:55:19:EA:31:33:80 | 1444738203 | Tue Oct 13 14:10:03 2015
//"-----+----------------------------------+-------+---------+-------+--------------------------+------------+--------------------------

//------------------------------------------------------------------
char tmpstr[64];
//------------------------------------------------------------------
//------------------------------------------------------------------
void wr_status_(DL_STATUS status, const char * pre_msg)
{
	puts(dbg_prn_status(status, pre_msg));
	fflush(stdout);
}

void print_percent_hdr_(void)
{
	int i;

	printf("\n%%:");
	for (i = 0; i < 101; ++i)
	{
		printf("%d", i % 10);
	}
	printf("\n%%=");
}

int getchar_(void)
{
	int selector;

	do
	{
		selector = getchar();
		//		printf("selector= %d | %c\n", selector, selector);
		//		fflush(stdout);

	} while (selector == '\n');

	return selector;
}

char * get_string(void)
{
	static char buff[4096] = "";
	char *result;
	int len;

	do
	{
		result = fgets(buff, sizeof(buff), stdin);
		len = strlen(buff);
		if (result != NULL && buff[len - 1] == '\n')
		{
			buff[len - 1] = '\0';
			if (len == 1)
				continue;
			else
				break;
		}
		else
		{
			// handle error
			puts("Error while getting line. Exit.");
			return 0;
		}
	} while (true);

	return buff;
}
//------------------------------------------------------------------

void get_info(DEV_HND dev)
{
	int hardware_type; // hardware type
	int firmware_version; // firmware version

	dev->status = AIS_GetVersion(dev->hnd, &hardware_type, &firmware_version);
	wr_status("AIS_GetVersion()");
	printf(" |>> hw= %d | fw= %d\n", hardware_type, firmware_version);

//	status = AIS_GetFTDISerial()
//	wr_status("AIS_GetVersion()"));

//	status = AIS_GetTime(

//	int battery_status;
//	int battery_available_percent;
//
//	status = AIS_BatteryGetInfo(dev->hnd, &battery_status,
//			&battery_available_percent);
//	printf("%s | %d | %d\n", wr_status("AIS_BatteryGetInfo()"),
//			battery_status, battery_available_percent);

	time_get(dev);
}

void password_change(DEV_HND dev)
{
	printf("Old password is actual application password: %s\n"
			"Enter new password for units ( and application ) : ", pass);
	fflush(stdout);

	scanf("%s", new_pass);

	if (!strlen(new_pass))
	{
		puts("Patch - default password = 1111");
		strcpy(new_pass, "1111");
	}

	printf("Try set new password for units= %s\n", new_pass);

	dev->status = AIS_ChangePassword(dev->hnd, pass, new_pass);
	printf("AIS_ChangePassword(pass: %s >> %s)> %s\n", pass, new_pass,
			dl_status2str(dev->status));

	if (!dev->status)
	{
		strcpy(pass, new_pass);
		printf("New default application password= %s\n\n", pass);
	}
}

void password_set_default(DEV_HND dev)
{
	printf("Actual application password is : %s\n"
			"Enter new default application password : ", pass);
	fflush(stdout);

	scanf("%s", pass);

	if (!strlen(pass))
	{
		puts("Patch - default unit password = 1111");
		strcpy(pass, "1111");
	}

	printf("New default application password= %s\n\n", pass);

}

void time_get(DEV_HND dev)
{
	uint64_t current_time = 0;
	int time_zone;
	int DST;
	int offset;

	dev->status = AIS_GetTime(dev->hnd, &current_time, &time_zone, &DST,
			&offset);
	if (dev->status)
	{
		wr_status("AIS_GetTime()");
		return;
	}

	printf("AIS_GetTime(dev=%p)> %s= (tz= %d | dst= %d | offset= %d)> %s\n\n",
			dev->hnd, dl_status2str(dev->status), time_zone, DST, offset,
			dbg_GMT2str(current_time));

#ifndef DEV_MIN_PRINTS
	puts(sys_get_timezone_info());
#endif
}

#define PRNVAR(var) printf( #var " (%d)= %d\n", (int) sizeof(var), (int) var);

void time_set(DEV_HND dev)
{
	uint64_t current_time = time(0);

// FIXME: resource busy
//	puts("Get before Set Time:");
//	time_get(dev->hnd);

#ifndef DEV_MIN_PRINTS
	puts(sys_get_timezone_info());
#endif

	int timezone = sys_get_timezone();
	int DST = sys_get_daylight();
	int offset = sys_get_dstbias();

	puts("..........................");
	puts("You may use this settings:");
	PRNVAR(timezone);
	PRNVAR(DST);
	PRNVAR(offset);
	puts("..........................");

	// TODO enter from keyboard
	dev->status = AIS_SetTime(dev->hnd, pass, current_time, timezone, DST,
			offset);

	printf("AIS_SetTime(pass:%s)> %s | (tz= %d | dst= %d | offset= %d) %s\n",
			pass, dl_status2str(dev->status), timezone, DST, offset,
			dbg_GMT2str(current_time));

//	if (!dev->status)
//	{
//		puts("Get after Set Time:");
//		time_get(dev->hnd);
//	}
}

void print_log_record(DEV_HND dev)
{
	int i;

	printf(prn_format, dev->log.index, dbg_action2str(dev->log.action),
			dev->log.reader_id, dev->log.card_id, dev->log.system_id);

	printf(" | [%1d]", dev->log.nfc_uid_len);
	for (i = 0; i < dev->log.nfc_uid_len; i++)
		printf(":%02X", dev->log.nfc_uid[i]);
	for (; i < 7; i++)
		printf("   ");

	printf(" | %s\n", dbg_GMT2str(dev->log.timestamp));
}

void print_log(DEV_HND dev)
{
//	int log_idx = 0;

	puts("-= PRINT LOG =-");
	puts(hdr[0]);
	puts(hdr[1]);
	puts(hdr[0]);

	do
	{
		dev->status = AIS_ReadLog(dev->hnd, &dev->log.index, &dev->log.action,
				&dev->log.reader_id, &dev->log.card_id, &dev->log.system_id,
				dev->log.nfc_uid, &dev->log.nfc_uid_len, &dev->log.timestamp);

		if (dev->status)
			break;

		print_log_record(dev);

//		dev->log.idx++;

	} while (true);

	puts(hdr[0]);

	wr_status("AIS_ReadLog()");
}

void print_rte(DEV_HND dev)
{
//	int log_idx = 0;

	int count = AIS_ReadRTE_Count(dev->hnd);

	printf("AIS_ReadRTE_Count= %d\n", count);

	puts("-= PRINT Real Time Events =-");
	puts(hdr[0]);
	puts(hdr[1]);
	puts(hdr[0]);

	do
	{
		dev->status = AIS_ReadRTE(dev->hnd, &dev->log.index, &dev->log.action,
				&dev->log.reader_id, &dev->log.card_id, &dev->log.system_id,
				dev->log.nfc_uid, &dev->log.nfc_uid_len, &dev->log.timestamp);

		if (dev->status)
			break;

		print_log_record(dev);

//		dev->log.idx++;

	} while (true);

	puts(hdr[0]);

	wr_status("AIS_ReadRTE()");
}

void log_get(DEV_HND dev)
{
	dev->status = AIS_GetLog(dev->hnd, pass);
	wr_status("AIS_GetLog_Set()");

	if (dev->status)
		return;

	DoCmd(dev);

	print_log(dev);
}

void log_get_by_index(DEV_HND dev)
{
	uint32_t start_index = 2;
	uint32_t end_index = 5;
	int r;

	puts("Read LOG by Index:");

	do
	{
		printf("Enter index start: ");
		fflush(stdout);
		r = scanf("%u", &start_index);
		printf("(start_index= %d / %#x) r = %d\n", start_index, start_index, r);
	} while (r != 1);

	do
	{
		printf("Enter index stop: ");
		fflush(stdout);
		r = scanf("%u", &end_index);
		printf("(end_index = %d / %#x) r = %d\n", end_index, end_index, r);
	} while (r != 1);

	dev->status = AIS_GetLogByIndex(dev->hnd, pass, start_index, end_index);

	printf("AIS_GetLogByIndex(pass: %s | [%d - %d]> %s\n", pass, start_index,
			end_index, dl_status2str(dev->status));

	DoCmd(dev);

	if (dev->status)
		return;

	print_log(dev);
}

void log_get_by_time(DEV_HND dev)
{
	uint32_t start_time = 1414670812;
	uint32_t end_time = 1414670830;
	int r;

	puts("Read LOG by Time (time-stamp) range:");

	do
	{
		printf("Enter time-stamp start: ");
		fflush(stdout);
		r = scanf("%u", &start_time);
		printf("(time-stamp start= %d / %#x) r = %d\n", start_time, start_time,
				r);
	} while (r != 1);

	do
	{
		printf("Enter time-stamp stop: ");
		fflush(stdout);
		r = scanf("%u", &end_time);
		printf("(time-stamp stop = %d / %#x) r = %d\n", end_time, end_time, r);
	} while (r != 1);

	dev->status = AIS_GetLogByTime(dev->hnd, pass, start_time, end_time);

	printf("AIS_GetLogByTime(pass: %s | [%d - %d]> %s\n", pass, start_time,
			end_time, dl_status2str(dev->status));

	DoCmd(dev);

	if (dev->status)
		return;

	print_log(dev);
}

void whitelist_read(DEV_HND dev)
{
	int white_list_size;
	const char *white_list = 0;

	puts("Read white-list :");

	dev->status = AIS_Whitelist_Read(dev->hnd, pass, &white_list);

	white_list_size = white_list ? strlen(white_list) : 0;

	printf("AIS_Whitelist_Read(pass:%s): size= %d > %s\n", pass,
			white_list_size, dl_status2str(dev->status));

	if (dev->status || white_list_size <= 0)
		return;

	puts(white_list);
}

void whitelist_write(DEV_HND dev)
{
	char *wl;

	puts("Try to write white-list:");
	puts("Enter white-list UIDs "
			"(in HEX format delimited with '.' or ':' or not)");
	puts("Each UID separate by ',' or space. "
			"Eg. 37:0C:96:69,C2.66.EF.95 01234567");
	printf("White-list UIDs: ");
	fflush(stdout);

	wl = get_string();
	if (!wl)
	{
		puts("Error while getting line of UIDs. Exit.");
		return;
	}

	dev->status = AIS_Whitelist_Write(dev->hnd, pass, wl);
	printf("AIS_Whitelist_Write(pass:%s)> %s\n", pass,
			dl_status2str(dev->status));
}

void blacklist_read(DEV_HND dev)
{
	int list_size = 0;
	c_string str_black_list = 0;

	puts("Read black-list :");

	dev->status = AIS_Blacklist_Read(dev->hnd, pass, &str_black_list);

	if (dev->status == DL_OK)
		list_size = strlen(str_black_list);

	printf("AIS_Blacklist_Read(pass:%s): black_list(size= %d | %p) > %s\n",
			pass, list_size, str_black_list, dl_status2str(dev->status));

	if (dev->status || list_size <= 0)
		return;

	puts(str_black_list);
}

void blacklist_write(DEV_HND dev)
{
	char *bl;

	puts("Try to write black-list decimal numbers (delimited with anything)");
	puts("Eg. 2, 102 250;11");
	printf("Enter Black-list numbers: ");
	fflush(stdout);

	bl = get_string();
	if (!bl)
	{
		puts("Error while getting line of numbers. Exit.");
		return;
	}

	dev->status = AIS_Blacklist_Write(dev->hnd, pass, bl);
	printf("AIS_Blacklist_Write(pass:%s)> %s\n", pass,
			dl_status2str(dev->status));
}

void print_log_unread(DEV_HND dev)
{
	printf("[%d] LOG unread (incremental)= %d\n", dev->idx, dev->LogUnread);
}

bool MainLoop(DEV_HND dev)
{
	bool print = false;

	dev->status = AIS_MainLoop(dev->hnd, //
			&dev->RealTimeEvents, &dev->LogAvailable, &dev->LogUnread, //
			&dev->cmdResponses, &dev->cmdPercent, //
			&dev->DeviceStatus, //
			&dev->TimeoutOccurred, &dev->Status);
	if (dev->status)
	{
		wr_status("AIS_MainLoop()");
		return false;
	}

	if (dev->RealTimeEvents)
	{
		printf("[%d] RTE= %d\n", dev->idx, dev->RealTimeEvents);
		print = true;

		print_rte(dev);
	}

	if (dev->LogAvailable)
	{
		printf("[%d] LOG= %d\n", dev->idx, dev->LogAvailable);
		print = true;

		print_log(dev);
	}

	if (dev->LogUnread_last != dev->LogUnread)
	{
		print_log_unread(dev);

		print = true;

//		print_log(dev);

		dev->LogUnread_last = dev->LogUnread;
	}

	if (dev->TimeoutOccurred)
	{
		printf("[%d] TimeoutOccurred= %d\n", dev->idx, dev->TimeoutOccurred);
		print = true;
	}

	if (dev->Status)
	{
		printf("[%d] local_status= %s\n", dev->idx,
				dl_status2str((DL_STATUS) dev->Status));
		print = true;
	}

	if (dev->cmdPercent)
	{
		if (dev->print_percent_hdr)
		{
			print_percent_hdr_();
			dev->percent_old = -1;

			dev->print_percent_hdr = false;
		}

		while (dev->percent_old != dev->cmdPercent)
		{
//			printf("AIS_DoCmd(): "
//					"cmd_finish= %d | percent= %3d (old= %d) || %s\n",
//					dev->cmdResponses, dev->cmdPercent, dev->percent_old,
//					dl_status2str(dev->status));

			if (dev->percent_old < 100)
				putchar('.');

			dev->percent_old++;

			print = true;
		}
	}

	if (dev->cmdResponses)
	{
		printf("\n-- [%d] COMMAND FINISH !\n", dev->idx);
		print = true;
	}

	if (print)
		fflush(stdout);

	return true;
}

void DoCmd(DEV_HND dev)
{
	dev->print_percent_hdr = true;

	do
	{
		if (!MainLoop(dev))
			break;

	} while (!dev->cmdResponses);
}

void rte_listen(DEV_HND dev, int seconds)
{
	int i;
	time_t stop_time = time(0) + seconds + 1;

	printf("...\n..\n.\nStart listen for RTE %d seconds...\n", seconds);
	fflush(stdout);

	while (time(0) < stop_time)
	{
		for (i = 0; i < device_count; ++i)
		{
			MainLoop(device[i]);

//		if (dev->status == NO_DEVICES)
//			break;
		}
	}

	printf("STOP listen for RTE.\n.\n");
}

void rte_listen_DEFTIME(DEV_HND dev)
{
	rte_listen(dev, DEFAULT_RTE_TIME);
}

//------------------------------------------------------------------
//-- get_unread_log_one

int get_unread_log_one__info(DEV_HND dev)
{
	int r;

	r = AIS_ReadLog_Count(dev->hnd);
	if (r)
		printf("AIS_ReadLog_Count()= %d\n", r);

	return r;
}

int get_unread_log_one__count(DEV_HND dev)
{
	int r = 0;

#if DLL_LESS_THEN_480

	uint32_t log_available;

	dev->status = AIS_UnreadLOG_Count(dev->hnd, &log_available);
	if (dev->status)
	{
		wr_status("AIS_UnreadLOG_Count()");
		return;
	}

	printf("AIS_UnreadLOG_Count()= log_available= %d\n", log_available);

#else
	MainLoop(dev);
	print_log_unread(dev);
#endif

	get_unread_log_one__info(dev);

	return r;
}

int get_unread_log_one__get(DEV_HND dev)
{
	puts("-= PRINT UNREAD LOG =-");
	puts(hdr[0]);
	puts(hdr[1]);
	puts(hdr[0]);

	do
	{
		dev->status = AIS_UnreadLOG_Get(dev->hnd, &dev->log.index,
				&dev->log.action, &dev->log.reader_id, &dev->log.card_id,
				&dev->log.system_id, dev->log.nfc_uid,
				&dev->log.nfc_uid_len, &dev->log.timestamp);

		if (dev->status)
			break;

		print_log_record(dev);

	} while (false);

	puts(hdr[0]);

	wr_status("AIS_UnreadLOG_Get()");

	if (dev->status)
	{
		return dev->status;
	}

	get_unread_log_one__info(dev);

	return dev->status;
}

int get_unread_log_one__ack(DEV_HND dev)
{
	dev->status = AIS_UnreadLOG_Ack(dev->hnd, 1);
	wr_status("AIS_UnreadLOG_Ack()");
	if (dev->status)
	{
		return dev->status;
	}

	get_unread_log_one__info(dev);

	return dev->status;
}

void get_unread_log_one__prn_help()
{
	puts("Help: use "
			"1 or c for count | "
			"2 or g for get | "
			"3 or a for ACK\n"
			"x/X/q/Q : exit from this part");
}

void get_unread_log_one(DEV_HND dev)
{
	bool cont = true;

	//--------------------------------------------------------------

	get_unread_log_one__prn_help();

	do
	{
		char ch = getchar();

		switch (ch)
		{
		case '1':
		case 'c':

			get_unread_log_one__count(dev);

			break;

		case '2':
		case 'g':

			get_unread_log_one__get(dev);

			break;

		case '3':
		case 'a':

			get_unread_log_one__ack(dev);

			break;

		case 'x':
		case 'X':
		case 'q':
		case 'Q':

			puts("exit from unread log");

			cont = false;

			return;

		case '\n':
			break; // skip

		default:

			get_unread_log_one__prn_help();

			break;
		}

		fflush(stdout);

	} while (cont);
}
//------------------------------------------------------------------

void get_io_state(DEV_HND dev)
{
	uint32_t intercom;
	uint32_t door;

	dev->status = AIS_GetIoState(dev->hnd, &intercom, &door, &dev->relay_state);
	if (dev->status)
	{
		wr_status("AIS_GetIoState()");
		return;
	}
	printf("IO STATE= intercom= %d, door= %d, relay_state= %d\n", intercom,
			door, dev->relay_state);
}

void relay_toggle(DEV_HND dev)
{
	get_io_state(dev);

	dev->relay_state = !dev->relay_state;

	dev->status = AIS_RelayStateSet(dev->hnd, dev->relay_state);

	sprintf(tmpstr, "AIS_RelayStateSet(RELAY= %d)", dev->relay_state);

	wr_status(tmpstr);
}

void lock_open(DEV_HND dev)
{
	uint32_t pulse_duration = 2000;

	dev->status = AIS_LockOpen(dev->hnd, pulse_duration);

	sprintf(tmpstr, "AIS_LockOpen(pulse_duration= %d ms)", pulse_duration);

	wr_status(tmpstr);
}

void test_light(DEV_HND dev)
{
	bool green_master = false;
	bool red_master = false;
	bool green_slave = false;
	bool red_slave = false;

	c_string hlp = "\n"
			"Tester Lights : "
			"g - green master | r - red master || "
			"G - green slave | R - red slave || "
			" || x - exit from the light testing\n";

	do
	{
		puts(hlp);
		fflush(stdout);

		switch (getchar_())
		{
		case 'g':
			green_master = !green_master;
			break;
		case 'r':
			red_master = !red_master;
			break;
		case 'G':
			green_slave = !green_slave;
			break;
		case 'R':
			red_slave = !red_slave;
			break;

		case 'x':
		case 'X':
		case 27:
			puts("Exit from test.");
			return;

		default:
			continue;
		}

		dev->status = AIS_LightControl(dev->hnd, green_master, red_master,
				green_slave, red_slave);

		printf("AIS_LightControl(green= %d | red= %d || "
				"slave:: green= %d | red= %d)> %s\n", green_master, red_master,
				green_slave, red_slave, dl_status2str(dev->status));

	} while (true);

}

void print_datatype_size(void)
{
	printf("sizeof(bool)= %d\n", (int) sizeof(bool));
}

void test_device(DEV_HND dev)
{
	get_info(dev);

#if 0
	puts("@@@@ START");
	puts("@@@@ START");
	puts("@@@@ START");
	puts("@@@@ START");
	rte_listen(dev, 10);
	log_get(dev);
	puts("@@@@ posle LOG");
	puts("@@@@ posle LOG");
	puts("@@@@ posle LOG");
	puts("@@@@ posle LOG");
	rte_listen(dev, 10);
	log_get(dev);
	puts("@@@@ posle LOG");
	puts("@@@@ posle LOG");
	puts("@@@@ posle LOG");
	puts("@@@@ posle LOG");
	rte_listen(dev, 10);
#endif
}

void dev_activate(unsigned int dev_id)
{
	if (dev_id < device_count)
	{
		device_active = device[dev_id];
		printf("Active device [%d] : Handle= %p : (%p)\n", dev_id + 1,
				device_active->hnd, device_active);
	}
	else
	{
		printf("No Device %d - Max number of devices= %d\n", dev_id + 1,
				device_count + 1);
	}
}
void dev_activate_1(DEV_HND dev)
{
	dev_activate(0);
}
void dev_activate_2(DEV_HND dev)
{
	dev_activate(1);
}
void dev_activate_3(DEV_HND dev)
{
	dev_activate(2);
}
void dev_activate_4(DEV_HND dev)
{
	dev_activate(3);
}

void print_menu();
void print_menu_x(DEV_HND dev)
{
	print_menu();
}

#define CMD_CNT				(sizeof(mn)/sizeof(*mn))

struct S_TEST_MENU
{
	char selector;
	const
	char * info;
	void (*fx)(DEV_HND device);
	bool must_have_device_handle;
} mn[] =
{
{ 'h', "Help / menu", print_menu_x, false },
{ 'x', "Exit from tester", 0, false },
{ 'Q', "Edit device list for checking", edit_device_list, false },
{ 'q', "List devices", list_device, false },
//{ 's', "select device", 0, true },
{ 'o', "Open device", open_device, true },
{ 'c', "Close device", close_device, true },
{ '1', "Device 1 activate", dev_activate_1, false },
{ '2', "Device 2 activate", dev_activate_2, false },
{ '3', "Device 3 activate", dev_activate_3, false },
//{ '4', "Device 4 activate", dev_activate_4, false },
{ 'i', "Device information", get_info, true },
{ 't', "Get time", time_get, true },
{ 'T', "Set time", time_set, true },
{ 'p', "Set application password", password_set_default, false },
{ 'P', "Change device password", password_change, true },
{ 'r', "Wait for RTE", rte_listen_DEFTIME, true },
{ 'l', "Get log", log_get, true },
{ 'n', "Get log by Index", log_get_by_index, true },
{ 'N', "Get log by Time", log_get_by_time, true },
{ 'u', "Get a unread LOG", get_unread_log_one, true },
{ 'w', "White-list Read", whitelist_read, true },
{ 'W', "White-list Write", whitelist_write, true },
{ 'b', "Black-list Read", blacklist_read, true },
{ 'B', "Black-list Write", blacklist_write, true },
{ 'L', "Test lights", test_light, true },
//{ 'T', "Test device function", test_device, true },
{ 'g', "Get IO state", get_io_state, true },
{ 'G', "Open gate/lock", lock_open, true },
{ 'y', "Relay toggle state", relay_toggle, true },

};

void print_menu()
{
	int i;

	puts("\n------------------------------");
	puts("Press key - select action :");

	for (i = 0; i < CMD_CNT; ++i)
	{
		if (!(i % MENU_COL_NUMBER))
			printf("\n");

		printf("%c : %s", mn[i].selector, mn[i].info);
		int slen = strlen(mn[i].info);
		int rest = MENU_COL_WIDTH - slen;
		while (rest-- > 0)
		{
			putchar(' ');
		}
	}
	puts("\n------------------------------");

	fflush(stdout);
}

int menu_switch(void)
{
	int selector;
	int i;

	do
	{
#ifndef DEV_MIN_PRINTS
		print_menu();
#endif // #ifndef DEV_MIN_PRINTS

		selector = getchar_();

		if (selector == 27) // ESC
			return 0;

		for (i = 0; i < CMD_CNT; ++i)
		{
			if (mn[i].selector == selector)
			{
				if (!mn[i].fx)
					return 0;

				if (!device_active && mn[i].must_have_device_handle)
				{
					puts("ERROR : NO ACTIVE DEVICE !!! TRY TO LIST DEVICES");
				}
				else
				{
					mn[i].fx(device_active);
				}

				break;
			}
		}

		fflush(stdout);

	} while (true);
}

int main(int argc, char **argv)
{
	int i;

	puts("");
	printf("Tester for 'ais_readers' dynamic library "
			"version %s and later\n", MINIMAL_LIB_VERSION);

	puts(AIS_GetLibraryVersionStr());

	print_datatype_size();

	list_device(0);

#if 0 // TEST

	dev_activate_1(0);
	open_device(device_active);
	dev_activate_2(0);
	open_device(device_active);

	dev_activate_1(0);
	time_get(device_active);
	dev_activate_2(0);
	time_get(device_active);

#else

	for (i = 0; i < device_count; ++i)
	{
		open_device(device[i]);
		time_get(device[i]);
	}

	menu_switch();

#endif

	destroy_devices();

	puts("EXIT.");

	return EXIT_SUCCESS;
}
