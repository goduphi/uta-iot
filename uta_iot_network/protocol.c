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

#define PREAMBLE            0xAA
#define BRIDGE_ADDRESS      0x42

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
    uint8_t* data = buffer + 7;
    data[0] = slotNumber;
    rfSendBuffer((uint8_t*)pH, 7 + nBytes);
}

bool isJoinResponse(uint8_t* buffer)
{
    packetHeader* pH = (packetHeader*)buffer;
    if(pH->preamble == PREAMBLE && pH->to == getDeviceId() && pH->messageType == (uint8_t)JOIN_RESP)
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
