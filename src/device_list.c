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

void prepare_list_for_check()
{
	DL_STATUS status;
	int device_type = DL_AIS_BASE_HD_SDK;
	int device_id = 0;

	puts("AIS_List_GetDevicesForCheck() BEFORE / DLL STARTUP");
	puts(AIS_List_GetDevicesForCheck());

	AIS_List_EraseAllDevicesForCheck();

	puts("Tester try to connect with Base HD devices on addresses 1 and 3 and 5");

	device_id = 1;

	status = AIS_List_AddDeviceForCheck(device_type, device_id);
	printf("AIS_List_AddDeviceForCheck(type: %d, id: %d)> { %s }\n",
			device_type, device_id, dl_status2str(status));

	device_id = 3;

	status = AIS_List_AddDeviceForCheck(device_type, device_id);
	printf("AIS_List_AddDeviceForCheck(type: %d, id: %d)> { %s }\n",
			device_type, device_id, dl_status2str(status));

	device_id = 5;

	status = AIS_List_AddDeviceForCheck(device_type, device_id);
	printf("AIS_List_AddDeviceForCheck(type: %d, id: %d)> { %s }\n",
			device_type, device_id, dl_status2str(status));

	puts("AIS_List_GetDevicesForCheck() AFTER LIST UPDATE");
	puts(AIS_List_GetDevicesForCheck());
}

void print_available_devices()
{
	puts("Look at ais_readers_list.h for Device enumeration");
	printf("Known devices ( supported by %s )\n", AIS_GetLibraryVersionStr());

	int i;
	c_string dev_name;
	c_string dev_dsc;
	DL_STATUS status;

	for (i = 0; i < DL_AIS_SYSTEM_TYPES_COUNT; ++i)
	{
		status = dbg_device_type(i, &dev_name, &dev_dsc, 0, 0, 0, 0, 0);
		printf("\tDevice type= %2d : ", i);
		if (status)
		{
			printf("NOT SUPPORTED!\n");
		}
		else
		{
#define SPCS		15

			printf("'%s'", dev_name);

			// column
			int spaces = strlen(dev_name);
			if (spaces <= SPCS)
				spaces = SPCS - spaces;
			while (spaces--)
				putchar(' ');

			printf("= '%s'\n", dev_dsc);
		}
	}
}

void edit_device_list(DEV_HND dev) // Parameter is irrelevant
{
	DL_STATUS status;
	int device_type;
	int device_id;
	int r;
	bool list_erased = false;

	puts("");
	puts("Edit device types for checking...");

	puts("AIS_List_GetDevicesForCheck() ACTUAL List");
	puts(AIS_List_GetDevicesForCheck());

	puts("Enter device type and then enter device BUS ID for check");
	print_available_devices();

	for (;;)
	{
		printf("Enter device type (1, 2, ... , %d) ('x' for exit): ",
				DL_AIS_SYSTEM_TYPES_COUNT - 1);
		fflush(stdout);
		r = scanf("%d", &device_type);
		if (!r)
			break;

		printf("Enter device bus ID (if full duplex then enter 0): ");
		fflush(stdout);
		r = scanf("%d", &device_id);
		if (!r)
		{
			fflush(stdin);
			device_id = 0;
		}

		if (!list_erased)
		{
			AIS_List_EraseAllDevicesForCheck();
			list_erased = true;
		}
		status = AIS_List_AddDeviceForCheck(device_type, device_id);
		printf("AIS_List_AddDeviceForCheck(type: %d, id: %d)> { %s }\n",
				device_type, device_id, dl_status2str(status));
		fflush(stdout);
	}

	puts("");
	puts("Finish list edit.");
	puts("");
	puts("AIS_List_GetDevicesForCheck() AFTER UPDATE");
	puts(AIS_List_GetDevicesForCheck());

	fflush(stdin);
}

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
