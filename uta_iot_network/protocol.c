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

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "protocol.h"


//----------------------------
// Protocol Functions
//----------------------------

/*
 * FIXME still working on checksum
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


void getChecksum(uint32_t sum)
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

*/
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

