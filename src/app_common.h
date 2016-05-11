/*
 * app_common.h
 *
 *  Created on: May 11, 2016
 *      Author: SrkoS
 */
#ifndef APP_COMMON_H_
#define APP_COMMON_H_

#include "device_struct.h"

//
extern char pass[64];


// common function
void wr_status_(DL_STATUS status, const char * pre_msg);
#define wr_status(pre_msg) wr_status_(dev->status,pre_msg)

// new:
//#define wr_status_1(s,m) puts(dbg_prn_status(s, m)); fflush(stdout);
//#define wr_status(pre_msg) wr_status_1(dev->status,pre_msg)

c_string time_now_str(void);

#endif /* APP_COMMON_H_ */
