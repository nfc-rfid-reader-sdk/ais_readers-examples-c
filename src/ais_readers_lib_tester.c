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
#ifndef __linux__
#	include <conio.h>
#endif

//#define DEBUG_ON

#ifndef DEBUG_ON
//#	define FAST_SELECT
#endif

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

//---------------------------------------------------------
static c_string prn_format = "%4d |%34s| %5d | %7d | %5d";
//---------------------------------------------------------

static c_string hdr[] =
{
  "-----+----------------------------------+-------+---------+-------+--------------------------+------------+--------------------------",
  " Idx |              action              | RD ID | Card ID | JobNr |    NFC [length] : UID    | Time-stamp |       Date - Time" };
//"-----+----------------------------------+-------+---------+-------+--------------------------+------------+--------------------------
//"   0 | [64(0x40):ACTION_CARD_UNLOCKED_5]|    19 |       0 |       | [7]:04:55:19:EA:31:33:80 | 1444738197 | Tue Oct 13 14:09:57 2015
//"   1 |[128(0x80):ACTION_CARD_UNLOCKED_5]|    19 | 1234567 | 12345 | [7]:04:55:19:EA:31:33:80 | 1444738203 | Tue Oct 13 14:10:03 2015
//"-----+----------------------------------+-------+---------+-------+--------------------------+------------+--------------------------

//---------------------------------------------------------
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

#ifdef __linux__
		selector = getchar();
#else // #ifdef __linux__
#	ifdef FAST_SELECT
		selector = getch();
#	else // #ifdef FAST_SELECT
		selector = getchar(); // for debug
#	endif // #ifdef FAST_SELECT
#endif // #ifdef __linux__
		//		printf("selector= %d | %c\n", selector, selector);
		//		fflush(stdout);

	} while (selector == '\n');

	return selector;
}

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

	dev->status = AIS_GetTime(dev->hnd, &current_time);
	if (dev->status)
		wr_status("AIS_GetTime()");
	else
#ifdef __linux__
		printf("AIS_GetTime()> %s= %lu | %s", dl_status2str(dev->status),
				current_time, ctime((time_t *) &current_time));

#else
		printf("AIS_GetTime()> %s= %I64u | %s", dl_status2str(dev->status),
				current_time, ctime((time_t *) &current_time));
#endif
}

void time_set(DEV_HND dev)
{
// FIXME: resource busy
//	puts("Get before Set Time:");
//	time_get(dev->hnd);

	dev->status = AIS_SetTime(dev->hnd, pass, time(0));

	printf("AIS_SetTime(pass:%s)> %s\n", pass, dl_status2str(dev->status));

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

#ifdef __linux__
	printf(" | %10lu | %s", dev->log.timestamp,
			ctime((time_t *) &dev->log.timestamp));
#else
	printf(" | %10I64u | %s", dev->log.timestamp,
			ctime((time_t *) &dev->log.timestamp));
#endif
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
	dev->status = AIS_GetLog_Set(dev->hnd, pass);
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
	char wl[4096] = "";
	char *result;
	int len;

	puts("Try to write white-list:");
	puts("Enter white-list UIDs "
			"(in HEX format delimited with '.' or ':' or not)");
	puts("Each UID separate by ',' or space. "
			"Eg. 37:0C:96:69,C2.66.EF.95 01234567");
	printf("White-list UIDs: ");
	fflush(stdout);

	do
	{
		result = fgets(wl, sizeof(wl), stdin);
		len = strlen(wl);
		if (result != NULL && wl[len - 1] == '\n')
		{
			wl[len - 1] = '\0';
			if (len == 1)
				continue;
			else
				break;
		}
		else
		{
			// handle error
			puts("Error while getting line of UIDs. Exit.");
			return;
		}
	} while (true);

	dev->status = AIS_Whitelist_Write(dev->hnd, pass, wl);
	printf("AIS_Whitelist_Write(pass:%s)> %s\n", pass,
			dl_status2str(dev->status));
}

void blacklist_read(DEV_HND dev)
{
	int list_size;
	char * black_list = 0;

	puts("Read black-list :");

	dev->status = AIS_Blacklist_GetSize(dev->hnd, pass, &list_size);
	printf("AIS_Blacklist_GetSize(pass:%s): size= %d > %s\n", pass,
			list_size, dl_status2str(dev->status));

	if (dev->status || list_size <= 0)
		return;

	black_list = (char *) malloc(list_size);

	AIS_Blacklist_Read(dev->hnd, black_list);

	puts(black_list);

	free(black_list);
}

void blacklist_write(DEV_HND dev)
{
	char bl[4096] = "";
	char *result;
	int len;

	puts("Try to write black-list decimal numbers (delimited with anything)");
	puts("Eg. 2, 102 250;11");
	printf("Enter Black-list numbers: ");
	fflush(stdout);

	do
	{
		result = fgets(bl, sizeof(bl), stdin);
		len = strlen(bl);
		if (result != NULL && bl[len - 1] == '\n')
		{
			bl[len - 1] = '\0';
			if (len == 1)
				continue;
			else
				break;
		}
		else
		{
			// handle error
			puts("Error while getting line of numbers. Exit.");
			return;
		}
	} while (true);

	dev->status = AIS_Blacklist_Write(dev->hnd, pass, bl);
	printf("AIS_Blacklist_Write(pass:%s)> %s\n", pass,
			dl_status2str(dev->status));
}

bool MainLoop(DEV_HND dev)
{
	bool print = false;

	dev->status = AIS_MainLoop(dev->hnd, &dev->RealTimeEvents,
			&dev->LogAvailable, &dev->cmdResponses, &dev->cmdPercent,
			&dev->TimeoutOccurred, &dev->Status);
	if (dev->status)
	{
		wr_status("AIS_MainLoop()");
		return false;
	}

	if (dev->RealTimeEvents)
	{
		printf("RTE= %d\n", dev->RealTimeEvents);
		print = true;

		print_rte(dev);
	}

	if (dev->LogAvailable)
	{
		printf("LOG= %d\n", dev->LogAvailable);
		print = true;

		print_log(dev);
	}

	if (dev->TimeoutOccurred)
	{
		printf("TimeoutOccurred= %d\n", dev->TimeoutOccurred);
		print = true;
	}

	if (dev->Status)
	{
		printf("local_status= %s\n", dl_status2str((DL_STATUS) dev->Status));
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
		puts("\n-- COMMAND FINISH !");
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
	time_t stop_time = time(0) + seconds + 1;

	printf("...\n..\n.\nStart listen for RTE %d seconds...\n", seconds);
	fflush(stdout);

	while (time(0) < stop_time)
	{
		MainLoop(dev);

		if (dev->status == NO_DEVICES)
			break;
	}

	printf("STOP listen for RTE.\n.\n");
	fflush(stdout);
}

void rte_listen_DEFTIME(DEV_HND dev)
{
	rte_listen(dev, DEFAULT_RTE_TIME);
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
			"G - green slave | r - red slave\n";

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
{ 'x', "Exit ( or press ESC )", 0, false },
{ 'q', "List devices", list_device, false },
//{ 's', "select device", 0, true },
{ 'o', "Open device", open_device, true },
{ 'c', "Close device", close_device, true },
{ 'i', "Device information", get_info, true },
{ 't', "Get time", time_get, true },
{ 'T', "Set time", time_set, true },
{ 'P', "Set default password in application", password_set_default, false },
{ 'p', "Change password", password_change, true },
{ 'r', "Wait for RTE", rte_listen_DEFTIME, true },
{ 'l', "Get log", log_get, true },
{ 'n', "Get log by Index", log_get_by_index, true },
{ 'N', "Get log by Time", log_get_by_time, true },
{ 'T', "Test device function", test_device, true },
{ 'w', "White-list Read", whitelist_read, true },
{ 'W', "White-list Write", whitelist_write, true },
{ 'b', "Black-list Read", blacklist_read, true },
{ 'B', "Black-list Write", blacklist_write, true },
{ 'L', "Test lights", test_light, true },
};

void print_menu()
{
	int i;
	const char menu_delimit[] =
#ifdef DEBUG_ON
			"\t\t";
#else
			"\n";
#endif

	puts("\n");
	puts("------------------------------");
	puts("------------------------------");
	puts("Press key - select action :");
	puts("------------------------------");

	for (i = 0; i < CMD_CNT; ++i)
	{
		printf("%c : %s%s", mn[i].selector, mn[i].info, menu_delimit);
	}
	puts("------------------------------");

	fflush(stdout);
}

int menu_switch(void)
{
	int selector;
	int i;

	do
	{
		print_menu();

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

	} while (true);
}

int main(int argc, char **argv)
{
	puts("Tester for 'ais_readers' dynamic library");

	puts(AIS_GetDLLVersion());

	print_datatype_size();

#warning Start inicijalizacija:

	list_device(0);
	if (device_active)
	{
		open_device(device_active);
		time_get(device_active);
	}

	menu_switch();

	destroy_devices();

	puts("EXIT.");

	return EXIT_SUCCESS;
}
