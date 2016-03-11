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
extern DEV_HND device_active;

#ifdef DEV_ON_HEAP

extern DEV_HND *device;
#define DEV_PTR(device_id_)	&device[device_id_]

#else // #ifdef DEV_ON_HEAP

#define MAX_DEVICES		50
extern device_t device[MAX_DEVICES];
#define DEV_PTR(device_id_)	&device[device_id_]

#endif // #ifdef DEV_ON_HEAP


void edit_device_list(DEV_HND dev);
void list_device(DEV_HND dev);

int destroy_devices(void);

void open_device(DEV_HND dev);
void close_device(DEV_HND dev);

#endif /* DEVICE_LIST_H_ */
