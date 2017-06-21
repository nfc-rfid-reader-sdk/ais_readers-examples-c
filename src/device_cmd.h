/*
 * device_cmd.h
 *
 *  Created on: May 10, 2016
 *      Author: SrkoS
 */
#ifndef DEVICE_CMD_H_
#define DEVICE_CMD_H_

#include "device_struct.h"

void time_get(DEV_HND dev);
void time_set(DEV_HND dev);

void ee_lock(DEV_HND dev);
void ee_unlock(DEV_HND dev);

#endif /* DEVICE_CMD_H_ */
