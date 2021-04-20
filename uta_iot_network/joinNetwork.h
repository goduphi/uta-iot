/*
 * joinButton.h
 *
 *  Created on: Apr 15, 2021
 *      Author: Sarker Nadir Afridi Azmi
 */

#ifndef JOINNETWORK_H_
#define JOINNETWORK_H_

#include <stdint.h>
#include <stdbool.h>
#include "gpio.h"

void initJoinNetwork(void);
/*
 * This function allows a device to be added to the network
 */
bool joinNetwork(void);

#endif /* JOINNETWORK_H_ */
