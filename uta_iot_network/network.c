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
#include "devices.h"

#define DEBUG

#ifdef DEBUG
    #include <stdio.h>
#endif

#define TX_LED      PORTF,1
#define RX_LED      PORTF,3

uint8_t timeIndex = 0;
uint32_t slottedTimes[] = {100, 40e6};
bool bridgeMode = true;

// Temporary variables
// These will be removed later
char out[50];
uint8_t sync[] = {0xCC};
uint8_t tmp[] = {0x12};

void initHw()
{
    enablePort(PORTF);
    selectPinPushPullOutput(TX_LED);
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
        if(bridgeMode)
            // Send the sync message
            rfSendBuffer(sync, 1);
        break;
    case 1:
        break;
    case 2:
        if(bridgeMode)
            //rfSetMode(RX);
        break;
    case 3:
        /*
        if(!bridgeMode)
        {
            putsUart0("Transmitting data ...\n");
            rfSetMode(TX);
            rfSendBuffer(tmp, 1);
            rfSetMode(RX);
        }
        */
        break;
    default:
        break;
    }

    if(timeIndex < 2)
    {
        stopTimer0();
        setTimerLoadValue(slottedTimes[timeIndex]);
        startTimer0();
    }

    timeIndex++;
    if(timeIndex == MAX_DEVICES)
    {
        if(bridgeMode)
            //rfSetMode(TX);
        timeIndex = 0;
    }

    clearTimer0InterruptFlag();
}

void initNetwork()
{
    initHw();
    initNrf24l01();
    rfSetAddress(RX_ADDR_P0, 0xABC);
    rfSetAddress(TX_ADDR, 0xABC);
    rfSetFrequency(37);

    initTimer0();
    setTimerLoadValue(0);

    if(bridgeMode)
    {
        putsUart0("TX Mode");
        rfSetMode(TX);
        startTimer0();
    }
    else
    {
        putsUart0("RX Mode");
        rfSetMode(RX);
    }

    while(true)
    {
        if(rfIsDataAvailable())
        {
            putsUart0("Received data ...\n");
            uint8_t data[10] = {0};
            uint32_t n = rfReceiveBuffer(data);

            if(data[0] == 0xCC)
            {
                setTimerLoadValue(0);
                startTimer0();
                sprintf(out, "Sync message (%d byte(s)): %x\n", n, data[0]);
                putsUart0(out);
            }
            else
            {
                sprintf(out, "Data (%d byte(s)): %x\n", n, data[0]);
                putsUart0(out);
            }
        }
    }
}
