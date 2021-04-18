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

 // FIXME still working on checksum
void sumWords(void* data, uint16_t sizeInBytes, uint32_t* sum)
{
    uint8_t* pData = (uint8_t*)data;
    uint16_t i;
    uint8_t phase = 0;
    uint16_t data_temp;
    for (i = 0; i < sizeInBytes; i++)
    {
        if (phase)
        {
            data_temp = *pData;
            *sum += data_temp << 8;
        }
        else
          *sum += *pData;
        phase = 1 - phase;
        pData++;
    }
}


uint32_t getChecksum(uint32_t sum)
{
    uint16_t result;
    // this is based on rfc1071
    while ((sum >> 16) > 0)
    {
        sum = (sum & 0xFFFF) + (sum >> 16);
    }
    result = sum & 0xFFFF;
    return ~result;
}

void calcProtocolChecksum(packetHeader *packet)
{
    uint8_t packetHeaderLength = (packet->preamble & 0xF) * 4;
    uint32_t sum = 0;
    // 32-bit sum over packet header
    packet->checksum = 0;
    sumWords(packet, packetHeaderLength, &sum);
    packet->checksum = getChecksum(sum);
}


// Converts from host to network order and vice versa
// smaller version
uint16_t htons(uint16_t value)
{
    return ((value & 0xFF00) >> 8) + ((value & 0x00FF) << 8);
}
#define ntohs htons

// larger version
uint32_t htonl(uint32_t value)
{
    return ((0xFF000000 & value) >> 24) + ((0x00FF0000 & value) >> 8) +
           ((0x0000FF00 & value) << 8) + ((0x000000FF & value) << 24);
}

#define ntohl htonl

// Determines if packet is using Custom Protocol (BAD VERSION BUT WORKS FOR NOW)
// this way doesn't use the checksum, only looks for preamble field
bool isProtocol(packetHeader *p)
{
    packetHeader* pH = (packetHeader*)p;
    bool ok;
    ok = (pH->preamble == PREAMBLE);            // check Preamble field to make sure it's Custom Protocol

    return ok;
}

// Gets pointer to Protocol payload of frame
uint8_t* getProtocolData(packetHeader *p)
{
    packetHeader* pH = (packetHeader*)p;
    return p->data;
}

/*
 * ORIGINAL WAY OF DETREMINING CUSTOM PROTOCOL
 * probably better to use once checksum works
 *
bool isProtocol(packetHeader *p)
{
    packetHeader* pH = (packetHeader*)p->data;
    uint8_t packetHeaderLength = (pH->preamble & 0xF) * 4;
    bool ok;
    uint16_t tmp16;
    uint32_t sum = 0;
    ok = (pH->preamble == 0xAA);            // check to make sure it's our custom Protocol

    if (ok)
    {
        // 32-bit sum over pseudo-header
        sumWords(p, (7 + pLength), &sum);
        tmp16 = p->preamble;
        tmp16 = htons(tmp16);
        sumWords(&tmp16, 1, &sum);
        // add udp header and data
        tmp16 = htons(pLength);
        sumWords(&tmp16, 1, &sum);
        sumWords(p, pLength, &sum);
        ok = (getChecksum(sum) == 0);
    }

    // calculate checksum and see if correct

    // preamble and checksum check to determine if valid Protocol packet
    return ok;
}
*/



