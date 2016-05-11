/*
 * app_common.c
 *
 *  Created on: May 11, 2016
 *      Author: SrkoS
 */

#include "app_common.h"

#include <stdio.h>
#include <time.h>

//##################################################################

char pass[64] = //
//		"55555";
		"1111";
//		"";

//##################################################################
// helper functions

void wr_status_(DL_STATUS status, const char * pre_msg)
{
	puts(dbg_prn_status(status, pre_msg));
	fflush(stdout);
}
//------------------------------------------------------------------

c_string time_now_str(void)
{
	static char text[64] = "";

	time_t rawtime = time(0);
	struct tm * timeinfo = localtime(&rawtime);
	strftime(text, sizeof(text), "%Y%m%d_%H%M%S", timeinfo);

	return text;
}
//------------------------------------------------------------------

//##################################################################
