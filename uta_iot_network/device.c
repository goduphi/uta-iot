/*
 * device.c
 *
 *  Created on: Apr 17, 2021
 *      Author: Sarker Nadir Afridi Azmi
 */

#include "device.h"
#include "protocol.h"

uint8_t deviceId = 0;
uint8_t deviceTimeSlot = 0;

void setDeviceId(uint8_t id)
{
    deviceId = id;
}

uint8_t getDeviceId()
{
    return deviceId;
}

void setDeviceTimeSlot(uint8_t timeSlot)
{
    deviceTimeSlot = timeSlot;
}
