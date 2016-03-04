/*
 * device_list.h
 *
 *  Created on: 14.10.2015.
 *      Author: SrkoS
 */
#ifndef DEVICE_LIST_H_
#define DEVICE_LIST_H_

#include "ais_readers_lib_tester.h"

extern int device_count;
extern DEV_HND *device;
extern DEV_HND device_active;

void edit_device_list(DEV_HND device);
void list_device(DEV_HND device);

int destroy_devices(void);

void open_device(DEV_HND dev);
void close_device(DEV_HND dev);

#endif /* DEVICE_LIST_H_ */
