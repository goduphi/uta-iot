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
#include "joinNetwork.h"
#include "wait.h"
#include "protocol.h"
#include "utils.h"
#include "device.h"
#include "common_terminal_interface.h"
#include "messageQueue.h"

#define DEBUG

#ifdef DEBUG
    #include <stdio.h>
#endif

#define TX_LED              PORTF,1
#define JOIN_LED            PORTF,2
#define RX_LED              PORTF,3

#define MAX_PACKET_SIZE     20
#define EMPTY_DEVICE        -1

#define NETWORK_ADDRESS     0xACCE55

// Global variables
extern bool isCarriageReturn;

uint8_t timeIndex = 0;
uint32_t slottedTimes[] = {20e6, 40e6, 120e6, 40e6};
bool accessSlot = false;
bool letJoin = false;
bool bridgeMode = false;

// Store the information of all connected devices
// The indexes will represent the id of the device
int8_t devices[MAX_DEVICES];
uint8_t deviceIndex = 0;

// This buffer will be the TX and RX buffer
uint8_t packet[MAX_PACKET_SIZE];

bool slotAssigned = false;
uint8_t currentDeviceTimeSlot = 0;
bool dataAvailable = true;

// Temporary variables
// These will be removed later
char out[50];
uint8_t tmp1[] = {0x77, 0x48, 0x69};
uint8_t tmp2[] = {0x77, 0x67, 0x23};

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

// Bridge functions - Start
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

uint8_t getNewDeviceId()
{
    packetHeader* pH = (packetHeader*)packet;
    return pH->from;
}

void addDevice(uint8_t id, uint8_t slotNumber)
{
    if(id >= MAX_DEVICES)
        return;
    devices[id] = slotNumber;
}
// Bridge functions - End

// Device functions - Start
uint8_t getTimeSlot()
{
    return (packet + 7)[0];
}
// Device functions - End

// This interrupt gets called any time we want to process data
void timer0Isr()
{
#ifdef DEBUG
    setPinValue(TX_LED, getPinValue(TX_LED)^1);
    sprintf(out, "Time index = %d\n", timeIndex);
    putsUart0(out);
#endif

    switch(timeIndex)
    {
    case 0:
        // Sync slot, size = 0
        if(bridgeMode)
            sendSync(packet, 0);
        break;
    case 1:
        // Down Link slot
        if(!qEmpty())
        {
            qnode message = pop();
            switch(message.type)
            {
            case JOIN_RESP:
                sendJoinResponse(packet, 1, message.id, deviceIndex);
                deviceIndex++;
                break;
            case PING_REQ:
                sendPingRequest(packet, message.id);
                break;
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
            rfSetMode(TX);
        break;
    case 3:
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

    // Offset the device time slot by 3 because the first three are taken up
    // by default
    if(!bridgeMode && (timeIndex == (currentDeviceTimeSlot + 3)) && dataAvailable && slotAssigned)
    {
        rfSetMode(TX);
        if(!qEmpty())
        {
            qnode message = pop();
            switch(message.type)
            {
            case PING_RESP:
                sendPingResponse(packet, getNewDeviceId(), getDeviceId());
                break;
            }
        }
        rfSetMode(RX);
        stopTimer0();
        clearTimer0InterruptFlag();
        timeIndex = 0;
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

    if(bridgeMode)
    {
        putsUart0("TX Mode\n");
        rfSetMode(TX);
        setTimerLoadValue(0);
        startTimer0();
    }
    else
    {
        putsUart0("RX Mode\n");
        rfSetMode(RX);
    }

    USER_DATA userData;

    // Endless receive loop
    while(true)
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
                    if(stringCompare("ping", getFieldString(&userData, 1)))
                    {
                        putsUart0("Sending ping ...\n");
                        qnode pingRequest = {getFieldInteger(&userData, 2), PING_REQ};
                        push(pingRequest);
                    }
                }

            }
        }

        // Keep on polling for the push button press
        // If pressed, let the user join for a short period of time
        if(joinNetwork())
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
            waitMicrosecond(1000000);
        }

        if(rfIsDataAvailable())
        {
            uint32_t n = rfReceiveBuffer(packet);
            sprintf(out, "Received %d byte(s) of data\n", n);
            putsUart0(out);

            if(n > 0)
            {
                switch(bridgeMode)
                {
                case true:
                    // Bridge receive section
                    if(letJoin)
                    {
                        if(isJoinRequest(packet))
                        {
                            uint8_t newDeviceId = getNewDeviceId();
                            sprintf(out, "Received join request\nNew device id = %d\n", newDeviceId);
                            putsUart0(out);
                            if(!deviceExists(newDeviceId))
                            {
                                addDevice(newDeviceId, deviceIndex);
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
                        putsUart0("Received ping response\n");
                    }

                    if(packet[0] == 0x77)
                    {
                        uint8_t i = 0;
                        for(i = 1; i < n; i++)
                        {
                            sprintf(out, "Data[%d] = %x\n", i, packet[i]);
                            putsUart0(out);
                        }
                    }

                    break;
                case false:
                    // Device receive section
                    if(isSync(packet))
                    {
                        // Reset everything
                        timeIndex = 0;
                        setTimerLoadValue(0);
                        startTimer0();
#ifdef DEBUG
                        putsUart0("Received sync\n");
#endif
                    }

                    if(isJoinResponse(packet))
                    {
                        putsUart0("Received a join response\n");
                        currentDeviceTimeSlot = getTimeSlot();
                        sprintf(out, "Time Slot = %d\n", currentDeviceTimeSlot);
                        putsUart0(out);
                        slotAssigned = true;
                    }

                    if(isPingRequest(packet))
                    {
                        putsUart0("Received ping request\n");
                        qnode pingResponse = {getDeviceId(), PING_RESP};
                        push(pingResponse);
                    }
                    break;
                }
            }
        }
    }
}
