/* protocol.h
 *
 * Author: Sean-Michael Woerner
 *
 * Description: Protocol header file that contains structures for packet construction of the custom protocol.
 *              This will include all structuring of packets and naming conventions for attributes of the structures
 *              and their fields.The RF24L01 module will be used for protocol communications.
 *
 * Target Board: Tiva C Series (TM4C123GH6PM)
 * Wireless Module: RF24L01
 * Stack Size: 4096
 */


#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

//----------------------------
// Protocol Enums
//----------------------------

// Type of message being sent
typedef enum _messageType
{
    SYNC = 0x00,
    JOIN_REQ = 0x01,
    JOIN_RESP = 0x02,
    PING_REQ = 0x03,
    PING_RESP = 0x04,
    CAPS_REQ = 0x05,
    CAPS_RESP = 0x06,
    PUSH_MSG = 0x07

} messageType;

//----------------------------
// Protocol Structures
//----------------------------

// Packet Header Structure
typedef struct _packetHeader    // 6 bytes + Data bytes
{
    uint8_t from;               // (1 byte) Where the packet is being sent from
    uint8_t to;                 // (1 byte) Where the packet is being sent to
    uint8_t msgType;            // (1 byte) Type of message (from enum messageType)
    uint8_t length;             // (1 byte) Length of packet header
    uint16_t checksum;          // (2 bytes) Checksum of packet header
    uint32_t data;              // (x bytes) Data being sent
} packetHeader;


//----------------------------
// Protocol Functions
//----------------------------
/*
void sumWords(void* data, uint16_t sizeInBytes, uint32_t* sum);
void getChecksum(uint32_t sum);
void calcProtocolChecksum(packetHeader *packet);
*/



