/*
 * device.c
 *
 *  Created on: Apr 17, 2021
 *      Author: Sarker Nadir Afridi Azmi
 */

#include "device.h"

uint8_t deviceId = 0;

/*
 * Device list
 * Store the information of all connected devices
 * The indexes will represent the id of the device
*
int8_t devices[MAX_DEVICES];
uint8_t deviceIndex = 0;

// Indicates if a time slot has been assigned to the device
bool slotAssigned = false;
uint8_t currentDeviceTimeSlot = 0;

*/
void setDeviceId(uint8_t id)
{
    deviceId = id;
}

uint8_t getDeviceId()
{
    return deviceId;
}

/*
uint8_t getNewDeviceId(uint8_t* packet)
{
    packetHeader* pH = (packetHeader*)packet;
    return pH->from;
}

void assignSlot()
{
    slotAssigned = true;
}

bool slotIsAssigned()
{
    return slotAssigned;
}

uint8_t getTimeSlot(uint8_t* packet)
{
    return (packet + 7)[0];
}

void setCurrentTimeSlot(uint8_t slot)
{
    currentDeviceTimeSlot = slot;
}

uint8_t getCurrentTimeSlot()
{
    return currentDeviceTimeSlot;
}

// Device list functions
void initDevices()
{
    uint8_t i = 0;
    for(i = 0; i < MAX_DEVICES; i++)
        devices[i] = EMPTY_DEVICE;
}

bool deviceExists(uint8_t id)
{
    return devices[id] != EMPTY_DEVICE;
}

void addDevice(uint8_t id, uint8_t slotNumber)
{
    if(id >= MAX_DEVICES)
        return;
    devices[id] = slotNumber;
}

uint8_t getDeviceIndex()
{
    return deviceIndex;
}

void incrementDeviceIndex()
{
    deviceIndex++;
}
*/
