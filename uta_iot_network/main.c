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
#include "nrf24l01.h"

int main(void)
{
    initSystemClockTo40Mhz();
    initUart0();
    setUart0BaudRate(115200, 40e6);
    initNetwork();

    // The device id will be a range from 0 - MAX_DEVICES
    setDeviceId(3);

    /*
     * Register the callback here
     */

    while(true)
    {
        commsReceive();
    }
}
