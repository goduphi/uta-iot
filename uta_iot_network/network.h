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

#define TX_LED              PORTF,1
#define JOIN_LED            PORTF,2
#define RX_LED              PORTF,3

void initNetwork();
void commsReceive();
void registerPushDataCallback(void (*callback)(uint8_t*));
bool getMode();

#endif /* NETWORK_H_ */
