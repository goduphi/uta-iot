/*
 * joinButton.c
 *
 *  Created on: Apr 15, 2021
 *      Author: Sarker Nadir Afridi Azmi
 */

#ifndef JOINBUTTON_C_
#define JOINBUTTON_C_

#include "joinNetwork.h"

#define PUSH_BUTTON     PORTF,4

void initJoinNetwork(void)
{
    enablePort(PORTF);
    selectPinDigitalInput(PUSH_BUTTON);
    enablePinPullup(PUSH_BUTTON);
}

bool joinNetwork(void)
{
    return !getPinValue(PUSH_BUTTON);
}

#endif /* JOINBUTTON_C_ */
