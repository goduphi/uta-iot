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

#define BRIDGE_ADDRESS      0x42
#define MAX_DEVICE_NAME     16
#define MAX_ATTRIBUTE_INFO  3
#define MAX_TOPIC_NAME      10

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
    PUSH_MSG = 0x07,
    DEV_CAPS1 = 0x08,
    DEV_CAPS2 = 0x09

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

typedef struct _attribute
{
    uint8_t id;
    char topicName[MAX_TOPIC_NAME + 1];
} attribute;

typedef struct _devCaps
{
    char deviceName[MAX_DEVICE_NAME];
    uint8_t attributeCount;
    attribute attributes[MAX_ATTRIBUTE_INFO];
} devCaps;

//----------------------------
// Protocol Functions
//----------------------------

void sendSync(uint8_t* buffer, uint8_t nBytes);
bool isSync(uint8_t* buffer);
void sendJoinRequest(uint8_t* buffer, uint8_t nBytes, uint8_t deviceId);
bool isJoinRequest(uint8_t* buffer);
void sendJoinResponse(uint8_t* buffer, uint8_t nBytes, uint8_t id, uint8_t slotNumber);
bool isJoinResponse(uint8_t* buffer);
void assembleDevCaps(uint8_t* buffer, char* deviceName, uint8_t attributeCount, uint8_t attributeId[], char* topicNames[]);
void sendDevCaps(uint8_t* buffer, uint8_t* devCapBuffer, messageType m);
bool isDevCap(uint8_t* buffer, messageType m);
void sendPingRequest(uint8_t* buffer, uint8_t deviceId);
bool isPingRequest(uint8_t* buffer);
void sendPingResponse(uint8_t* buffer, uint8_t id, uint8_t deviceId);
bool isPingResponse(uint8_t* packet);
void pushData(uint8_t* buffer, uint8_t* data, uint8_t from, uint8_t to, uint8_t nBytes);
bool isPushData(uint8_t* packet, uint8_t address);

#endif /* PROTOCOL_H_ */
