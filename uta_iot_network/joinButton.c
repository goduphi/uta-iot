/*
 * joinButton.c
 *
 *  Created on: Apr 15, 2021
 *      Author: Sarker Nadir Afridi Azmi
 */

#ifndef JOINBUTTON_C_
#define JOINBUTTON_C_

#include "joinButton.h"

#define PUSH_BUTTON     (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 4*4)))

void joinNetwork()
{
    while(PUSH_BUTTON);
}

#endif /* JOINBUTTON_C_ */
