/*
 * devices.h
 *
 *  Created on: Apr 15, 2021
 *      Author: Sarker Nadir Afridi Azmi
 */

#ifndef DEVICES_H_
#define DEVICES_H_

#include <stdint.h>

#define MAX_DEVICES     15

typedef struct device
{
    uint8_t index;
    uint8_t deviceIds[MAX_DEVICES];
} device;

#endif /* DEVICES_H_ */
