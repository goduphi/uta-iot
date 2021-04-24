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

typedef void (*_sendCallback)(void);

void initNetwork();
void registerSendCallback(void (*s)(uint8_t*, uint8_t), uint8_t* data, uint8_t length);

#endif /* NETWORK_H_ */
