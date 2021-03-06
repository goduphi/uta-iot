/* protocol.h
 *
 * Author: Sean-Michael Woerner
 * Edited by: Sarker Nadir Afridi Azmi
 *
 * Description: Protocol header file that contains structures for packet construction of the custom protocol.
 *              This will include all structuring of packets and naming conventions for attributes of the structures
 *              and their fields.The RF24L01 module will be used for protocol communications.
 *
 * Target Board: Tiva C Series (TM4C123GH6PM)
 * Wireless Module: RF24L01
 * Stack Size: 4096
 */

#ifndef PROTOCOL_H_
#define PROTOCOL_H_

#include <stdint.h>
#include <stdbool.h>

//----------------------------
// Protocol Enumerations
//----------------------------

// Type of message being sent
typedef enum _messageType
{
    SYNC = 0xBB,
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
typedef struct _packetHeader    // 7 bytes + Data bytes
{
    uint8_t preamble;           // (1 byte) Makes sure that this is the correct packet
    uint8_t from;               // (1 byte) Where the packet is being sent from
    uint8_t to;                 // (1 byte) Where the packet is being sent to
    uint8_t messageType;        // (1 byte) Type of message (from enum messageType)
    uint8_t length;             // (1 byte) Length of packet header
    uint16_t checksum;          // (2 bytes) Checksum of packet header
    uint8_t data[0];
} packetHeader;

//----------------------------
// Protocol Functions
//----------------------------

void sendSync(uint8_t* buffer, uint8_t nBytes);
bool isSync(uint8_t* buffer);
void sendJoinRequest(uint8_t* buffer, uint8_t nBytes, uint8_t deviceId);
bool isJoinRequest(uint8_t* buffer);
void sendJoinResponse(uint8_t* buffer, uint8_t nBytes, uint8_t id, uint8_t slotNumber);
bool isJoinResponse(uint8_t* buffer);
void sendPingRequest(uint8_t* buffer, uint8_t deviceId);
bool isPingRequest(uint8_t* buffer);
void sendPingResponse(uint8_t* buffer, uint8_t id, uint8_t deviceId);
bool isPingResponse(uint8_t* packet);
void sumWords(void* data, uint16_t sizeInBytes, uint32_t* sum);
uint16_t getChecksum(uint32_t sum);
void calcProtocolChecksum(packetHeader *packet);
uint16_t htons(uint16_t value);
uint32_t htonl(uint32_t value);




#endif /* PROTOCOL_H_ */
