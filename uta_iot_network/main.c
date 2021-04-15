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
#include "uart0.h"
#include "network.h"

int main(void)
{
    initSystemClockTo40Mhz();
    initUart0();
    setUart0BaudRate(115200, 40e6);
    initNetwork();
    while(true);
}
