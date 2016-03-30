/*
 * device_list.c
 *
 *  Created on: 14.10.2015.
 *      Author: SrkoS
 */

#include "device_list.h"
#include <ais_readers_list.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int device_count;
DEV_HND device_active = NULL;

#ifdef DEV_ON_HEAP
extern DEV_HND *device;
#else // #ifdef DEV_ON_HEAP
#define MAX_DEVICES		50
device_t device[MAX_DEVICES];
#endif // #ifdef DEV_ON_HEAP

char tmpstr[512];

void open_device(DEV_HND dev)
{
	sprintf(tmpstr, "AIS_Open(%p)", dev->hnd);
	dev->status = AIS_Open(dev->hnd);
	wr_status(tmpstr);
}

void close_device(DEV_HND dev)
{
	sprintf(tmpstr, "AIS_Close(%p)", dev->hnd);
	dev->status = AIS_Close(dev->hnd);
	wr_status(tmpstr);
}

// device_count
int create_devices(void)
{
#ifdef DEV_ON_HEAP

	int i;
	int devices_to_create;
	static bool created_once = false;

	if (created_once)
		return 0;

	if (device)
		destroy_devices();

//	devices_to_create = device_count;
	devices_to_create = device_count > 10 ? device_count + 10 : 10;

	device = calloc(devices_to_create, sizeof(DEV_HND));

	for (i = 0; i < devices_to_create; i++)
	{
		device[i] = malloc(sizeof(device_t));
		memset(device[i], 0, sizeof(device_t));
	}

	created_once = true;

#endif // #ifdef DEV_ON_HEAP

	return 0; // OK
}

int destroy_devices(void)
{
	int i;

	// free mem
	for (i = 0; i < device_count; ++i)
		if (DEV_PTR(i))
		{
			close_device(DEV_PTR(i));
#ifdef DEV_ON_HEAP
			free(device[i]);
#endif // #ifdef DEV_ON_HEAP
		}

#ifdef DEV_ON_HEAP
	if (device)
		free(device);
#endif // #ifdef DEV_ON_HEAP

	return 0;
}

void get_list_info(void)
{
	DL_STATUS status;
	c_string Device_Serial; //// device serial number
	int Device_Type; //// device type
	int Device_ID; //// device identification number (master)
	int Device_FW_VER; //// version of firmware
	int Device_CommSpeed; //
	c_string Device_FTDI_Serial; //// FTDI COM port identification
	int Device_Status; //// actual device status
	int System_Status;
	int i;
	c_string hdr[2] = {
			"----!------------------+----------+----------+-----+-----++--------+----------++--------+-----------+-----------",
			"idx |  Reader HANDLE   | SerialNm | Type h/d | ID  | FW  || speed  | FTDI: sn || opened | DevStatus | SysStatus",
	//	     ----!------------------+----------+----------+-----+-----++--------+----------++--------+-----------+-----------
	//	     123 | 0000000003391040 | ab012345 | 14h (20) | 20  | 112 || 250000 | A6Z27CHD || 0      | 0         | 30
	};
	c_string format = "%3d | %16p | %8s | %2Xh (%d) | %3i | %3i || %6u | %8s || %6d | %9d | %d\n";

	// check all connected readers
	// for description, types, serials

	create_devices();

//	printf("device_count= %d | count= %d\n", device_count, count);

	puts(hdr[0]);
	puts(hdr[1]);
	puts(hdr[0]);
	for (i = 0; i < device_count; i++)
	{
		DEV_HND dh = DEV_PTR(i);

		dh->idx = i + 1;

		// serial, Type, GetFTDISerial, FTDIDescription, HND
		status = AIS_List_GetInformation(&dh->hnd, &Device_Serial, &Device_Type,
				&Device_ID, &Device_FW_VER, &Device_CommSpeed,
				&Device_FTDI_Serial, &dh->open, &Device_Status,
				&System_Status);

		if (status)
		{
			printf("AIS_List_GetInformation: status= %s\n",
					dbg_status2str(status));
			continue;
		}

		printf(format, dh->idx, dh->hnd, Device_Serial, Device_Type,
				Device_Type, Device_ID, Device_FW_VER, Device_CommSpeed,
				Device_FTDI_Serial, dh->open, Device_Status,
				System_Status);
	}
	puts(hdr[0]);
	puts(".");

#ifdef DEV_AUTO_OPEN

	for (i = 0; i < device_count; ++i)
	{
		DEV_HND dh = DEV_PTR(i);

		if (dh->open)
			continue;

		open_device(dh);

		time_get(dh);
	}

#endif // #ifdef DEV_AUTO_OPEN

	device_active = DEV_PTR(0);
	puts("Device [1] is selected for active");
}

void add_device(int device_type, int device_id)
{
	DL_STATUS status;

	status = AIS_List_AddDeviceForCheck(device_type, device_id);
	printf("AIS_List_AddDeviceForCheck(type: %d, id: %d)> { %s }\n",
			device_type, device_id, dl_status2str(status));
}

void prepare_list_for_check()
{
	puts("AIS_List_GetDevicesForCheck() BEFORE / DLL STARTUP");
	puts(AIS_List_GetDevicesForCheck());

	AIS_List_EraseAllDevicesForCheck();

	puts("Tester try to connect with a Base HD device on any/unknown ID");
	add_device(DL_AIS_BASE_HD_SDK, 0);

	puts("AIS_List_GetDevicesForCheck() AFTER LIST UPDATE");
	puts(AIS_List_GetDevicesForCheck());
}

void print_known_device_types()
{
	int i;
	c_string dev_name;
	c_string dev_dsc;
	DL_STATUS status;
	const char *hdr_l = "---------+-----------------+--------------------------------";
	const char *hdr_t = "Dev.type |   Short  name   | Long name";

	puts("");
	puts(hdr_l);
	puts("Look at ais_readers_list.h for Device enumeration");
	printf("Known devices ( supported by %s )\n", AIS_GetLibraryVersionStr());
	puts(hdr_l);
	puts(hdr_t);
	puts(hdr_l);

	for (i = 1; i < DL_AIS_SYSTEM_TYPES_COUNT; ++i)
	{
		status = dbg_device_type(i, &dev_name, &dev_dsc, 0, 0, 0, 0, 0);
		printf("%8d : ", i);
		if (status)
		{
			printf("NOT SUPPORTED!\n");
		}
		else
		{
			printf("%15s | %s\n", dev_name, dev_dsc);
		}
	}
	puts(hdr_l);
}

void edit_device_list__help()
{
	puts("Help : Edit device list");
	puts("(no case sensitive)");

	puts(" h / 0 : print this help");

	puts(" x / q / Q : exit from sub menu");

	puts(" 1 / t : show known device types");

	puts(" 2 / l : show actual list for checking");

	puts(" 3 / c : clear list for checking");

	puts(" 4 / + / a : add device for check");

	puts(" 5 / - / e : erase device from checking list");
}

void do_dev_action(DL_API DL_STATUS (*dev_f)(int type, int id))
{
	DL_STATUS status;
	int r;
	int device_type;
	int device_id;

	puts("Enter device type and then enter device BUS ID for check");

	printf("Enter device type (1, 2, ... , %d) ('x' for exit): ",
			DL_AIS_SYSTEM_TYPES_COUNT - 1);
	fflush(stdout);
	r = scanf("%d", &device_type);
	if (!r)
		return;

	printf("Enter device bus ID (if full duplex then enter 0): ");
	fflush(stdout);
	r = scanf("%d", &device_id);
	if (!r)
	{
		fflush(stdin);
		device_id = 0;
	}

	status = dev_f(device_type, device_id);
	printf("(type: %d, id: %d)> { %s }\n",
			device_type, device_id, dl_status2str(status));
}

void dev_list_print(const char *msg)
{
	if (msg)
		puts(msg);
	else
		puts("Show actual list for checking:");

	puts(AIS_List_GetDevicesForCheck());
}

void edit_device_list(DEV_HND dev) // Parameter is irrelevant
{
	bool prn_start = true;
	//--------------------------------------------------------------
	fflush(stdin);

	puts("");
	puts("Edit device types for checking - sub-menu:");

	//--------------------------------------------------------------
	edit_device_list__help();

	for(;;)
	{
		if (prn_start)
		{
			puts("Enter sub menu command, and hit enter : ");
			prn_start = false;
		}

		fflush(stdout);

		char ch = tolower(getchar());

		switch (ch)
		{
		case 'x':
		case 'q':

			puts("");
			puts("Finish list edit.");
			puts("");
			dev_list_print("AFTER UPDATE CYCLE");

			fflush(stdout);
			fflush(stdin);

			return;

		case '1':
		case 't':

			print_known_device_types();

			break;

		case '2':
		case 'l':

			dev_list_print(0);

			break;

		case '3':
		case 'c':

			puts("Clear list for checking !");
			AIS_List_EraseAllDevicesForCheck();

			break;

		case '4':
		case 'a':
		case '+':

			printf("AIS_List_AddDeviceForCheck()...\n");
			do_dev_action(AIS_List_AddDeviceForCheck);


			dev_list_print(0);

			break;

		case '5':
		case 'e':
		case '-':

			printf("AIS_List_EraseDeviceForCheck()...\n");
			do_dev_action(AIS_List_EraseDeviceForCheck);

			dev_list_print(0);

			break;

		case '\n':
			prn_start = true;
			// puts("new line");

			break; // skip

		case '0':
		case 'h':
		default:

			edit_device_list__help();

			break;
		}
	}
}
//------------------------------------------------------------------

void list_device(DEV_HND dev) // Parameter is irrelevant
{
	static bool init_list = false;

	if (!init_list)
	{
		prepare_list_for_check();
		init_list = true;
	}

	puts("checking... please wait...");

	device_count = AIS_List_UpdateAndGetCount();

	printf("AIS_List_UpdateAndGetCount()= %d\n", device_count);

	if (device_count)
		get_list_info();
	else
		puts("NO DEVICE FOUND !");

	fflush(stdout);
}
