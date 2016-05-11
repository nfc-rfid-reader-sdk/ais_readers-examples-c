/*
 * device_cmd.c
 *
 *  Created on: May 10, 2016
 *      Author: SrkoS
 */

#include "device_cmd.h"

#include "app_common.h"
#include <stdio.h>

//##################################################################
//##################################################################

void ee_lock(DEV_HND dev)
{
	dev->status = AIS_EE_WriteProtect(dev->hnd, pass);
	wr_status("EEPROM Lock - AIS_EE_WriteProtect()");
}

void ee_unlock(DEV_HND dev)
{
	dev->status = AIS_EE_WriteUnProtect(dev->hnd, pass);
	wr_status("EEPROM Unlock - AIS_EE_WriteUnProtect()");
}

//------------------------------------------------------------------

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

//#ifndef DEV_MIN_PRINTS
//	puts(sys_get_timezone_info());
//#endif
}
