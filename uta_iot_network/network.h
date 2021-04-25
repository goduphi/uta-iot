/*
 * network.h
 *
 *  Created on: Apr 14, 2021
 *      Author: afrid
 */

#ifndef NETWORK_H_
#define NETWORK_H_

#include <stdint.h>
#include <stdbool.h>

void initNetwork();
void commsReceive();
void registerSendCallback(void (*callback)(void));

#endif /* NETWORK_H_ */
