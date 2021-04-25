/**
 * Author: Sarker Nadir Afridi Azmi
 *
 * Description: n/a
 *
 * Target Board: Tiva C Series (TM4C123GH6PM)
 * Stack Size: 4096
 */

#include <stdint.h>
#include <stdbool.h>
#include "clock.h"
#include "gpio.h"
#include "uart0.h"
#include "network.h"
#include "device.h"
#include "messageQueue.h"
#include "protocol.h"

void receiveData(uint8_t* buffer)
{
    putsUart0("Turning on led ...\n");
    packetHeader* pH = (packetHeader*)buffer;
    uint8_t* data = (buffer + 7);
    if(data[0] == 1)
        setPinValue(JOIN_LED, 1);
}

int main(void)
{
    initSystemClockTo40Mhz();
    initUart0();
    setUart0BaudRate(115200, 40e6);
    initNetwork();

    // The device id will be a range from 0 - MAX_DEVICES
    setDeviceId(4);

    /*
     * Register the callback here
     */
    if(!getMode())
        registerPushDataCallback(receiveData);

    while(true)
    {
        commsReceive();
    }
}
