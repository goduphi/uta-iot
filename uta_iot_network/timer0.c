/*
 * timer0.c
 *
 *  Created on: Apr 13, 2021
 *      Author: Sarker Nadir Afridi Azmi
 *
 *  This timer controls the synching of devices connected to the network
 */

#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"
#include "timer0.h"

void initTimer0()
{
    // Enable clocks
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R0;
    _delay_cycles(3);

    // Configure Timer 1 as the time base
    TIMER0_CTL_R &= ~TIMER_CTL_TAEN;                 // turn-off timer before reconfiguring
    TIMER0_CFG_R = TIMER_CFG_32_BIT_TIMER | TIMER_TAMR_TACDIR; // configure as 32-bit timer (A+B)
    TIMER0_TAMR_R = TIMER_TAMR_TAMR_PERIOD;          // configure for periodic mode (count down)
    TIMER0_IMR_R = TIMER_IMR_TATOIM;                 // turn-on interrupts
    NVIC_EN0_R |= 1 << (INT_TIMER0A-16);             // turn-on interrupt 37 (TIMER1A)
}

void startTimer0()
{
    TIMER0_CTL_R |= TIMER_CTL_TAEN;                  // turn-on timer
}

void stopTimer0()
{
    TIMER0_CTL_R &= ~TIMER_CTL_TAEN;                  // turn-on timer
}

void setTimerLoadValue(uint32_t loadValue)
{
    TIMER0_TAILR_R = loadValue;
}

void clearTimer0InterruptFlag()
{
    TIMER0_ICR_R = TIMER_ICR_TATOCINT;
}

