/*
 * device_list.c
 *
 *  Created on: 14.10.2015.
 *      Author: SrkoS
 */

#include "device_list.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int device_count;
DEV_HND *device = NULL;
DEV_HND device_active = NULL;

void open_device(DEV_HND dev)
{
	dev->status = AIS_Open(dev->hnd);
	wr_status("AIS_Open()");
}

void close_device(DEV_HND dev)
{
	dev->status = AIS_Close(dev->hnd);
	wr_status("AIS_Close()");
}

// device_count
int create_devices(void)
{
	int i;

	if (device)
		destroy_devices();

	device = calloc(device_count, sizeof(DEV_HND));

	for (i = 0; i < device_count; i++)
	{
		device[i] = malloc(sizeof(device_t));
		memset(device[i], 0, sizeof(device_t));
	}

	return 0; // OK
}

int destroy_devices(void)
{
	int i;

	// free mem
	for (i = 0; i < device_count; ++i)
		if (device[i])
		{
			close_device(device[i]);
			free(device[i]);
		}

	if (device)
		free(device);

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
	int Device_isOpened; //
	int Device_Status; //// actual device status
	int System_Status;
	int i;
	c_string hdr[2] = {
			"----!------------------+----------+----------+-----+-----++--------+----------++--------+-----------+-----------",
			"idx |  Reader HANDLE   | SerialNm | Type h/d | ID  | FW  || speed  | FTDI: sn || opened | DevStatus | SysStatus",
	//	     ----!------------------+----------+----------+-----+-----++--------+----------++--------+-----------+-----------
	//	     123 | 0000000003391040 | ab012345 | 14h (20) | 20  | 112 || 250000 | A6Z27CHD || 0      | 0         | 30
	};
	c_string format = "%3d | %16p | %8s | %Xh (%d) | %3i | %3i || %6u | %8s || %6d | %9d | %d\n";

	// check all connected readers
	// for description, types, serials

	create_devices();

//	printf("device_count= %d | count= %d\n", device_count, count);

	puts(hdr[0]);
	puts(hdr[1]);
	puts(hdr[0]);
	for (i = 0; i < device_count; i++)
	{
		// serial, Type, GetFTDISerial, FTDIDescription, HND
		status = AIS_List_GetInformation(&device[i]->hnd, &Device_Serial,
				&Device_Type, &Device_ID, &Device_FW_VER, &Device_CommSpeed,
				&Device_FTDI_Serial, &Device_isOpened, &Device_Status,
				&System_Status);

		if (status)
		{
			printf("AIS_List_GetInformation: status= %s\n",
					dbg_status2str(status));
			continue;
		}

		printf(format, i + 1, device[i]->hnd, Device_Serial,
				Device_Type, Device_Type, Device_ID, Device_FW_VER,
				Device_CommSpeed, Device_FTDI_Serial, Device_isOpened,
				Device_Status, System_Status);
	}
	puts(hdr[0]);
	puts(".");

	device_active = device[0];
	puts("Device [1] is selected for active");
}

void prepare_list_for_check()
{
	DL_STATUS status;
	int device_type = DL_BASE_HD;
	int device_id = 0;

	puts("AIS_List_GetDevicesForCheck() BEFORE / DLL STARTUP");
	puts(AIS_List_GetDevicesForCheck());

	AIS_List_EraseAllDevicesForCheck();

	puts("TODO: command for enter devices for checking !");
	puts("Tester try to connect with Base HD devices on addresses 1 and 3");

	device_id = 1;

	status = AIS_List_AddDeviceForCheck(device_type, device_id);
	printf("AIS_List_AddDeviceForCheck(type: %d, id: %d)> { %s }\n",
			device_type, device_id, dl_status2str(status));

	device_id = 3;

	status = AIS_List_AddDeviceForCheck(device_type, device_id);
	printf("AIS_List_AddDeviceForCheck(type: %d, id: %d)> { %s }\n",
			device_type, device_id, dl_status2str(status));

	puts("AIS_List_GetDevicesForCheck() AFTER LIST UPDATE");
	puts(AIS_List_GetDevicesForCheck());
}

void list_device(DEV_HND device) // Parameter is irrelevant
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
