/*
 * ais_readers_lib_tester.h
 *
 *  Created on: 08.10.2015.
 *      Author: SrkoS
 *
 * ABCDEFGHIJKLMNOPQRSTUVWXYZ
 * -*-****----+-*-*****--*---
 * -**-*-***--*-********-***-
 * abcdefghijklmnopqrstuvwxyz
 *
 * 1234567890
 * ****----------------------
 *
 */
#ifndef AIS_READERS_LIB_TESTER_H_
#define AIS_READERS_LIB_TESTER_H_

#include "device_cmd.h"

// define in IDE
//#define DEV_MIN_PRINTS

#define USE_THREADED_TEST
//#define USE_LOCK_MAINLOOP

void get_list_info(void);

void get_info(DEV_HND dev);

void password_change(DEV_HND dev);


void DoCmd(DEV_HND dev);

//--------------------

/**
 *
 * @param skip_enter if true - f can return only enter
 *
 * @return
 */
char * get_string(bool skip_enter);

#endif /* AIS_READERS_LIB_TESTER_H_ */
