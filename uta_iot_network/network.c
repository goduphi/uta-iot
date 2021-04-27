/*
 * network.c
 *
 *  Created on: Apr 14, 2021
 *      Author: Sarker Nadir Afridi Azmi
 */

#include <stdint.h>
#include <stdbool.h>
#include "clock.h"
#include "gpio.h"
#include "nrf24l01.h"
#include "timer0.h"
#include "uart0.h"
#include "network.h"
#include "joinNetwork.h"
#include "wait.h"
#include "protocol.h"
#include "utils.h"
#include "device.h"
#include "common_terminal_interface.h"
#include "messageQueue.h"
#include "eeprom.h"

#define DEBUG

#ifdef DEBUG
    #include <stdio.h>
#endif

#define MAX_PACKET_SIZE     128

#define NETWORK_ADDRESS     0xACCE55

// Global variables
extern bool isCarriageReturn;

uint8_t timeIndex = 0;
uint8_t backoffCounter = 0;
uint32_t slottedTimes[] = {1e6, 2e6, 40e6, 2e6};

bool accessSlot = false;
bool joinPressed = false;
bool letJoin = false;

bool bridgeMode = false;

// This buffer will be the TX and RX buffer
uint8_t packet[MAX_PACKET_SIZE];
uint8_t globalOffset = 0;

uint8_t devCapsData[MAX_PACKET_SIZE];
uint8_t dataReceivedIndex = 0;
bool receivedDevCap = false;

uint8_t receivedBridgeAddress = 0;

uint8_t deviceDataBuffer[MAX_PACKET_SIZE];
uint8_t deviceDataLength = 0;

bool isPressed = false;
void (*pushDataReceiveCallback)(uint8_t*);

// Temporary variables
// These will be removed later
char out[MAX_PACKET_SIZE];
pair bindTable[MAX_DEVICES];
uint8_t bindIndex = 0;

char* topicNames[10] = {"led"};
uint8_t topicIds[] = {0x02};

/*
 * Private functions
 */
void initHw()
{
    enablePort(PORTF);
    selectPinPushPullOutput(TX_LED);
    selectPinPushPullOutput(JOIN_LED);
    selectPinPushPullOutput(RX_LED);
}

// Device initialization
#define USER_SWITCH2                PORTF,0

void initPushButton()
{
    enablePort(PORTF);
    enablePinPullup(USER_SWITCH2);
    setPinCommitControl(USER_SWITCH2);
    selectPinDigitalInput(USER_SWITCH2);
}

// This interrupt gets called any time we want to process data
void timer0Isr()
{
    switch(timeIndex)
    {
    case 0:
        // Sync slot, size = 0
        if(bridgeMode)
        {
            sendSync(packet, 0);
            waitMicrosecond(18);
        }
        break;
    case 1:
        if(bridgeMode)
        {
            // Down Link slot
            if(!qEmpty())
            {
                qnode message = pop();
                switch(message.type)
                {
                case JOIN_RESP:
                    putsUart0("Sending join response ...\n");
                    sendJoinResponse(packet, 1, message.id, getDeviceTimeSlot(message.id));
                    break;
                case PING_REQ:
                    putsUart0("Sending ping ...\n");
                    sendPingRequest(packet, message.id);
                    break;
                case PUSH_MSG:
                    putsUart0("Pushing data ...\n");
                    pushData(packet, deviceDataBuffer, BRIDGE_ADDRESS, message.id, deviceDataLength);
                    break;
                }
            }
        }
        break;
    case 2:
        // Access Slot
        if(bridgeMode)
        {
            rfSetMode(RX);
            accessSlot = true;
        }
        else
        {
            rfSetMode(TX);
        }
        joinPressed = true;
        break;
    case 3:
        joinPressed = false;
        if(bridgeMode)
        {
            accessSlot = false;
            letJoin = false;
            setPinValue(JOIN_LED, 0);
        }
        break;
    default:
        break;
    }

    setPinValue(TX_LED, getPinValue(TX_LED)^1);

#ifdef DEBUG
    // sprintf(out, "Time index = %d\n", timeIndex);
    // putsUart0(out);
#endif

    // Offset the device time slot by 3 because the first three are taken up
    // by default
    if(!bridgeMode && (timeIndex == (getCurrentTimeSlot() + 3)) && deviceSlotIsAssigned())
    {
        if(!qEmpty())
        {
            qnode message = pop();
            switch(message.type)
            {
            case PING_RESP:
                putsUart0("Sending ping response ...\n");
                sendPingResponse(packet, receivedBridgeAddress, message.id);
                break;
            case PUSH_MSG:
                putsUart0("Sending new data ...\n");
                pushData(packet, deviceDataBuffer, message.id, BRIDGE_ADDRESS, deviceDataLength);
                isPressed = false;
                break;
            case DEV_CAPS1:
                putsUart0("Sending dev cap 1 ...\n");
                sendDevCaps(packet, devCapsData, DEV_CAPS1);
                qnode devCaps2 = {BRIDGE_ADDRESS, DEV_CAPS2};
                push(devCaps2);
                break;
            case DEV_CAPS2:
                putsUart0("Sending dev cap 2 ...\n");
                sendDevCaps(packet, devCapsData, DEV_CAPS2);
                break;
            }
        }
    }

    if(timeIndex < 4)
    {
        stopTimer0();
        setTimerLoadValue(slottedTimes[timeIndex]);
        startTimer0();
    }

    timeIndex++;
    if(timeIndex == MAX_DEVICES)
    {
        if(bridgeMode)
            rfSetMode(TX);
        else
            rfSetMode(RX);
        timeIndex = 0;
    }

    clearTimer0InterruptFlag();
}

void bridgeReceivePushData(uint8_t* packet)
{
    packetHeader* pH = (packetHeader*)packet;
    uint8_t i = 0;
    for(i = 0; i < bindIndex; i++)
    {
        if(bindTable[i].id1 == pH->from)
        {
            deviceDataBuffer[0] = (*(packet + 7));
            deviceDataLength = pH->length;
            qnode tmp = {bindTable[i].id2, PUSH_MSG};
            push(tmp);
        }
    }
}

void registerPushDataCallback(void (*callback)(uint8_t*))
{
    pushDataReceiveCallback = callback;
}

void tmp(uint8_t* packet) {}

bool getMode()
{
    return bridgeMode;
}

/*
 * Public functions
 */

void initNetwork()
{
    initHw();
    initNrf24l01();
    rfSetAddress(RX_ADDR_P0, NETWORK_ADDRESS);
    rfSetAddress(TX_ADDR, NETWORK_ADDRESS);
    initJoinNetwork();
    initTimer0();
    initDevices();
    stopTimer0();
    initPushButton();

    if(bridgeMode)
    {
        putsUart0("TX Mode\n");
        rfSetMode(TX);
        registerPushDataCallback(bridgeReceivePushData);
        setTimerLoadValue(0);
        startTimer0();
    }
    else
    {
        assembleDevCaps(devCapsData, "Bulb", 1, topicIds, topicNames);
        putsUart0("RX Mode\n");
        rfSetMode(RX);
    }
}

void commsReceive()
{
    USER_DATA userData;

    // Endless receive loop
    // while(true)
    {
        if(kbhitUart0())
        {
            getsUart0(&userData);

            if(isCarriageReturn)
            {
                isCarriageReturn = false;
                parseField(&userData);

                // Insert command functions here
                if(isCommand(&userData, "send", 1))
                {
                    if(bridgeMode && stringCompare("ping", getFieldString(&userData, 1)))
                    {
                        uint8_t deviceId = getFieldInteger(&userData, 2);
                        if(!deviceExists(deviceId))
                            putsUart0("Device does not exist\n");
                        else
                        {
                            qnode pingRequest = {deviceId, PING_REQ};
                            push(pingRequest);
                        }
                    }

                    // This is here for testing purposes
                    if(bridgeMode && stringCompare("push", getFieldString(&userData, 1)))
                    {
                        uint8_t deviceId = getFieldInteger(&userData, 2);
                        if(!deviceExists(deviceId))
                            putsUart0("Device does not exist\n");
                        else
                        {
                            deviceDataBuffer[0] = 2;
                            deviceDataLength = 1;
                            qnode pushData = {deviceId, PUSH_MSG};
                            push(pushData);
                        }
                    }
                }

                if(bridgeMode && isCommand(&userData, "bind", 2))
                {
                    putsUart0("Binding ...\n");
                    pair p = {getFieldInteger(&userData, 1), getFieldInteger(&userData, 2)};
                    bindTable[bindIndex++] = p;
                }

                if(!bridgeMode && isCommand(&userData, "clearep", 0))
                {
                    putsUart0("Erasing EEPROM ...\n");
                    writeEeprom(DEVICE_DATA_START, 0xFFFFFFFF);
                }
            }
        }

        // Insert send functions
        if(!bridgeMode && !isPressed && !getPinValue(USER_SWITCH2))
        {
            isPressed = true;
            putsUart0("Push button pressed\n");

            deviceDataBuffer[0] = 1;
            deviceDataLength = 1;
            qnode data = {getDeviceId(), PUSH_MSG};
            push(data);
        }

        // Keep on polling for the push button press
        // If pressed, let the user join for a short period of time
        if(joinNetwork() && joinPressed)
        {
            if(bridgeMode && accessSlot)
            {
                putsUart0("Press join button to pair\n");
                letJoin = true;
                setPinValue(JOIN_LED, 1);
            }
            else
            {
                putsUart0("Sending join request ...\n");
                sendJoinRequest(packet, 0, getDeviceId());
            }
            joinPressed = false;
        }

        if(rfIsDataAvailable())
        {
            uint32_t n = rfReceiveBuffer(packet);

            if(n > 0)
            {
                if(bridgeMode)
                {
                    // Bridge receive section
                    if(letJoin)
                    {
                        if(isJoinRequest(packet))
                        {
                            uint8_t newDeviceId = getNewDeviceId(packet);
                            sprintf(out, "Received join request\nNew device id = %d\n", newDeviceId);
                            putsUart0(out);

                            if(!deviceExists(newDeviceId))
                            {
                                addDevice(newDeviceId, getDeviceIndex());
                                incrementDeviceIndex();
                                qnode joinResponse = {newDeviceId, JOIN_RESP};
                                push(joinResponse);
                            }
                            else
                            {
                                putsUart0("Device already exists\n");
                            }
                        }
                    }

                    if(isPingResponse(packet))
                    {
                        sprintf(out, "Received ping response from device id = %d\n", getNewDeviceId(packet));
                        putsUart0(out);
                    }

                    if(isPushData(packet, BRIDGE_ADDRESS))
                        pushDataReceiveCallback(packet);

                    // IMPORTANT: Nathan's parsing code goes here
                    if(isDevCaps(packet, DEV_CAPS1))
                    {
                        putsUart0("Received dev cap 1\n");
                        uint8_t i = 0;
                        for(i = 0; i < 25; i++)
                            devCapsData[i] = (packet + 7)[i];
                    }

                    if(isDevCaps(packet, DEV_CAPS2))
                    {
                        putsUart0("Received dev cap 2\n");
                        uint8_t i = 0;
                        for(i = 0; i < 31; i++)
                            devCapsData[i + 25] = (packet + 2)[i];

                        devCaps* dC = (devCaps*)devCapsData;
                        sprintf(out, "Device name = %s, Attribute count = %d\n", dC->deviceName, dC->attributeCount);
                        putsUart0(out);
                        for(i = 0; i < dC->attributeCount; i++)
                        {
                            sprintf(out, "%d. Attr = %s, Id = %d\n", (i + 1), dC->attributes[i].topicName, dC->attributes[i].id);
                            putsUart0(out);
                        }
                    }
                }
                else
                {
                    // Device receive section
                    if(isSync(packet))
                    {
                        // Reset everything
                        timeIndex = 0;
                        setPinValue(TX_LED, 0);
                        setTimerLoadValue(0);
                        startTimer0();
                    }

                    if(isJoinResponse(packet))
                    {
                        putsUart0("Received a join response\nWriting data to Eeprom ...\n");
                        // setCurrentTimeSlot(getTimeSlot(packet));
                        uint8_t timeSlotReceived = getTimeSlot(packet);
                        writeDeviceMetaData(getDeviceId(), timeSlotReceived);
                        setCurrentTimeSlot(timeSlotReceived);
                        assignDeviceSlot();
                        // Send all the device capabilities
                        qnode initialDevCaps = {BRIDGE_ADDRESS, DEV_CAPS1};
                        push(initialDevCaps);

                        sprintf(out, "Device id = %d, Time Slot = %d\n", getDeviceId(), getCurrentTimeSlot());
                        putsUart0(out);
                    }

                    if(isPingRequest(packet))
                    {
                        putsUart0("Received ping request\n");
                        qnode pingResponse = {getDeviceId(), PING_RESP};
                        push(pingResponse);
                        receivedBridgeAddress = getNewDeviceId(packet);
                    }

                    if(isPushData(packet, getDeviceId()))
                        pushDataReceiveCallback(packet);
                }
            }
        }
    }
}
