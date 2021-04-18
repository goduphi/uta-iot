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

#define DEBUG

#ifdef DEBUG
    #include <stdio.h>
#endif

#define TX_LED              PORTF,1
#define JOIN_LED            PORTF,2
#define RX_LED              PORTF,3

#define MAX_DEVICES         10
#define MAX_PACKET_SIZE     20

uint8_t timeIndex = 0;
uint32_t slottedTimes[] = {20e6, 40e6, 120e6, 40e6};
bool accessSlot = false;
bool letJoin = false;
bool bridgeMode = false;

// All registered send callback functions go here
void (*sendCallbacks[MAX_DEVICES])(uint8_t*, uint8_t);
uint8_t sendCallbackIndex = 0;

uint8_t packet[MAX_PACKET_SIZE];
uint8_t packetLength = 0;

uint8_t sendBuffer[MAX_PACKET_SIZE];

// Temporary variables
// These will be removed later
char out[50];

/*
 * This is just for reference
    if(!bridgeMode && !dev2)
    {
        putsUart0("Transmitting ...\n");
        rfSetMode(TX);
        rfSendBuffer(tmp, sizeof(tmp));
        waitMicrosecond(130);
        rfSetMode(RX);
        timeIndex = 0;
        stopTimer0();
        clearTimer0InterruptFlag();
        return;
    }
*/

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

    if(timeIndex > 2)
    {
        sendCallbacks[timeIndex](sendBuffer, packetLength);
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

void defaultSendCallback(uint8_t* data, uint8_t length)
{
}

void initSendCallbacks()
{
    uint8_t i = 0;
    for(i = 0; i < MAX_DEVICES; i++)
    {
        sendCallbacks[i] = defaultSendCallback;
    }
}

/*
 * Public functions
 */
void registerSendCallback(void (*s)(uint8_t*, uint8_t), uint8_t* data, uint8_t length)
{
    if(sendCallbackIndex >= MAX_DEVICES)
    {
        putsUart0("Cannot add more devices.\n");
        return;
    }
    sendCallbacks[sendCallbackIndex++ + 3] = s;
    copyArray(data, sendBuffer, length);
    packetLength = length;
}

void initNetwork()
{
    initHw();
    initNrf24l01();
    rfSetAddress(RX_ADDR_P0, 0xABC);
    rfSetAddress(TX_ADDR, 0xABC);
    initJoinNetwork();
    initTimer0();
    initSendCallbacks();

    if(bridgeMode)
    {
        putsUart0("TX Mode");
        rfSetMode(TX);
        setTimerLoadValue(0);
        startTimer0();
    }
    else
    {
        putsUart0("RX Mode");
        rfSetMode(RX);
    }

    while(true)
    {
        // Keep on polling for the push button press
        // If pressed, let the user join for a short period of time
        if(joinNetwork())
        {
            if(bridgeMode && accessSlot)
            {
                putsUart0("Press join button to pair.\n");
                letJoin = true;
                setPinValue(JOIN_LED, 1);
            }
            else
            {
                putsUart0("Transmitting ...\n");
                // Send device information
                uint8_t id[] = {0x69};
                rfSendBuffer(id, 1);
            }
            waitMicrosecond(1000000);
        }

        if(rfIsDataAvailable())
        {
            uint32_t n = rfReceiveBuffer(packet);
            sprintf(out, "Received %d byte(s) of data ...\n", n);
            putsUart0(out);

            if(n > 0)
            {
                if(letJoin)
                {
                    // Store device data
                    sprintf(out, "Received new device: %x\n", packet[0]);
                    putsUart0(out);
                }

                if(isSync(packet))
                {
                    timeIndex = 0;
                    setTimerLoadValue(0);
                    startTimer0();
#ifdef DEBUG
                    putsUart0("Received sync.\n");
#endif
                }
            }
        }
    }
}
