/*
 * messageQueue.h
 *
 *  Created on: Apr 19, 2021
 *      Author: Sarker Nadir Afridi Azmi
 */

#ifndef MESSAGEQUEUE_H_
#define MESSAGEQUEUE_H_

#include <stdint.h>
#include <stdbool.h>
#include "device.h"
#include "protocol.h"

typedef struct _qnode
{
    uint8_t id;
    messageType type;
} qnode;

bool qFull();
bool qEmpty();
void push(qnode q);
qnode pop();

#endif /* MESSAGEQUEUE_H_ */
