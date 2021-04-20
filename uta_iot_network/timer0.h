/*
 * timer0.h
 *
 *  Created on: Apr 13, 2021
 *      Author: Sarker Nadir Afridi Azmi
 */

#ifndef TIMER0_H_
#define TIMER0_H_

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

void initTimer0();
void startTimer0();
void stopTimer0();
void setTimerLoadValue(uint32_t loadValue);
void clearTimer0InterruptFlag();

#endif /* TIMER0_H_ */
