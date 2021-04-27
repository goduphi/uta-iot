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
#include "eeprom.h"

void receiveData(uint8_t* buffer)
{
    putsUart0("Turning on led ...\n");
    uint8_t* data = (buffer + 7);

    if(data[0] == 1)
        setPinValue(JOIN_LED, 1);
}

void initCurrentDevice()
{
    uint32_t deviceMetaData = readEeprom(DEVICE_DATA_START);
    if(deviceMetaData == 0xFFFFFFFF)
    {
        // The device id will be a range from 0 - MAX_DEVICES
        setDeviceId(2);
    }
    else
    {
        setDeviceId(deviceMetaData & 0xFF);
        setCurrentTimeSlot((deviceMetaData >> 8) & 0xFF);
        assignDeviceSlot();
        putsUart0("Loading device meta data ...\n");
    }
}

int main(void)
{
    initSystemClockTo40Mhz();
    initEeprom();
    initUart0();
    setUart0BaudRate(115200, 40e6);
    initNetwork();

    /*
     * Register the callback here
     */
    if(!getMode())
    {
        initCurrentDevice();
        registerPushDataCallback(receiveData);
    }

    while(true)
    {
        commsReceive();


    }
}
