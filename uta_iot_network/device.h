/*
 * devices.h
 *
 *  Created on: Apr 15, 2021
 *      Author: Sarker Nadir Afridi Azmi
 */

#ifndef DEVICE_H_
#define DEVICE_H_

#include <stdint.h>
#include <stdbool.h>
#include "protocol.h"

#define MAX_DEVICES         10
//#define EMPTY_DEVICE        -1

void setDeviceId(uint8_t id);
uint8_t getDeviceId();
/*
uint8_t getNewDeviceId(uint8_t* packet);
void assignSlot();
bool slotIsAssigned();
uint8_t getTimeSlot(uint8_t* packet);
void setCurrentTimeSlot(uint8_t slot);
uint8_t getCurrentTimeSlot();

void initDevices();
bool deviceExists(uint8_t id);
void addDevice(uint8_t id, uint8_t slotNumber);
uint8_t getDeviceIndex();
void incrementDeviceIndex();
*/

#endif /* DEVICE_H_ */
