/* protocol.c
 *
 * Author: Sean-Michael Woerner
 *
 * Description: Protocol .c file that contains functions used by the custom protocol.
 *              The RF24L01 module will be used for protocol communications.
 *
 * Target Board: Tiva C Series (TM4C123GH6PM)
 * Wireless Module: RF24L01
 * Stack Size: 4096
 */

#include <stdint.h>
#include <stdbool.h>
#include "nrf24l01.h"
#include "protocol.h"
#include "device.h"
#include "common_terminal_interface.h"

#define PREAMBLE            0xAA

//----------------------------
// Protocol Functions
//----------------------------

void sendSync(uint8_t* buffer, uint8_t nBytes)
{
    packetHeader* pH = (packetHeader*)buffer;
    pH->preamble = PREAMBLE;
    pH->from = BRIDGE_ADDRESS;
    pH->to = 0xFF;
    pH->messageType = (uint8_t)SYNC;
    pH->length = nBytes;
    pH->checksum = 0;
    rfSendBuffer((uint8_t*)pH, 7);
}

bool isSync(uint8_t* buffer)
{
    packetHeader* pH = (packetHeader*)buffer;
    if(pH->preamble == PREAMBLE && pH->from == BRIDGE_ADDRESS && pH->messageType == (uint8_t)SYNC)
        return true;
    return false;
}

void sendJoinRequest(uint8_t* buffer, uint8_t nBytes, uint8_t deviceId)
{
    packetHeader* pH = (packetHeader*)buffer;
    pH->preamble = PREAMBLE;
    pH->from = deviceId;
    pH->to = 0xFF;
    pH->messageType = (uint8_t)JOIN_REQ;
    pH->length = nBytes;
    pH->checksum = 0;
    rfSendBuffer((uint8_t*)pH, 7);
}

bool isJoinRequest(uint8_t* buffer)
{
    packetHeader* pH = (packetHeader*)buffer;
    if(pH->preamble == PREAMBLE && pH->messageType == (uint8_t)JOIN_REQ)
        return true;
    return false;
}

void sendJoinResponse(uint8_t* buffer, uint8_t nBytes, uint8_t id, uint8_t slotNumber)
{
    packetHeader* pH = (packetHeader*)buffer;
    pH->preamble = PREAMBLE;
    pH->from = BRIDGE_ADDRESS;
    pH->to = id;
    pH->messageType = (uint8_t)JOIN_RESP;
    pH->length = nBytes;
    pH->checksum = 0;
    (*(buffer + 7)) = slotNumber;
    rfSendBuffer((uint8_t*)pH, 7 + nBytes);
}

bool isJoinResponse(uint8_t* buffer)
{
    packetHeader* pH = (packetHeader*)buffer;
    if(pH->preamble == PREAMBLE && pH->to == getDeviceId() && pH->messageType == (uint8_t)JOIN_RESP)
        return true;
    return false;
}

void assembleDevCaps(uint8_t* buffer, char deviceName, uint8_t attrCount, uint8_t attribute, char topicName)
{
}

void sendDevCaps(uint8_t* buffer, char* deviceName, uint8_t attributeId[], char* topicName[])
{
    packetHeader* pH = (packetHeader*)buffer;
    pH->preamble = PREAMBLE;
    pH->from = getDeviceId();
    pH->to = BRIDGE_ADDRESS;
    pH->messageType = (uint8_t)DEV_CAPS;
    pH->length = sizeof(devCaps);
    pH->checksum = 0;

    devCaps* data = (devCaps*)(buffer + 7);
    strCpy(deviceName, data->deviceName);

    uint8_t i = 0;
    for(i = 0; i < 3; i++)
    {
        data->attributes[i].id = attributeId[i];
        strCpy(topicName[i], data->attributes[i].topicName);
    }

    rfSendBuffer((uint8_t*)pH, 32);
}

bool isDevCap(uint8_t* buffer)
{
    packetHeader* pH = (packetHeader*)buffer;
    if(pH->preamble == PREAMBLE && pH->to == BRIDGE_ADDRESS && pH->messageType == (uint8_t)DEV_CAPS)
        return true;
    return false;
}

void sendPingRequest(uint8_t* buffer, uint8_t deviceId)
{
    packetHeader* pH = (packetHeader*)buffer;
    pH->preamble = PREAMBLE;
    pH->from = BRIDGE_ADDRESS;
    pH->to = deviceId;
    pH->messageType = (uint8_t)PING_REQ;
    pH->length = 0;
    pH->checksum = 0;
    rfSendBuffer((uint8_t*)pH, 7);
}

bool isPingRequest(uint8_t* buffer)
{
    packetHeader* pH = (packetHeader*)buffer;
    if(pH->preamble == PREAMBLE && pH->to == getDeviceId() && pH->messageType == (uint8_t)PING_REQ)
        return true;
    return false;
}

void sendPingResponse(uint8_t* buffer, uint8_t id, uint8_t deviceId)
{
    packetHeader* pH = (packetHeader*)buffer;
    pH->preamble = PREAMBLE;
    pH->from = deviceId;
    pH->to = id;
    pH->messageType = (uint8_t)PING_RESP;
    pH->length = 0;
    pH->checksum = 0;
    rfSendBuffer((uint8_t*)pH, 7);
}

bool isPingResponse(uint8_t* packet)
{
    packetHeader* pH = (packetHeader*)packet;
    if(pH->preamble == PREAMBLE && pH->to == BRIDGE_ADDRESS && pH->messageType == (uint8_t)PING_RESP)
        return true;
    return false;
}

void pushData(uint8_t* buffer, uint8_t* data, uint8_t from, uint8_t to, uint8_t nBytes)
{
    packetHeader* pH = (packetHeader*)buffer;
    pH->preamble = PREAMBLE;
    pH->from = from;
    pH->to = to;
    pH->messageType = (uint8_t)PUSH_MSG;
    pH->length = nBytes;
    pH->checksum = 0;
    uint8_t i = 0;
    for(i = 0; i < nBytes; i++)
    {
        (*(buffer + 7 + i)) = data[i];
    }
    rfSendBuffer((uint8_t*)pH, 7 + nBytes);
}

bool isPushData(uint8_t* packet, uint8_t address)
{
    packetHeader* pH = (packetHeader*)packet;
    if(pH->preamble == PREAMBLE && pH->to == address && pH->messageType == (uint8_t)PUSH_MSG)
        return true;
    return false;
}
