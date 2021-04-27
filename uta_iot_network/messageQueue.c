/*
 * messageQueue.c
 *
 *  Created on: Apr 19, 2021
 *      Author: Sarker Nadir Afridi Azmi
 */

#include "messageQueue.h"

qnode messageQueue[MAX_DEVICES];
uint8_t head = 0;
uint8_t tail = 0;

bool qFull()
{
    return (tail > head) && (tail%MAX_DEVICES == head%MAX_DEVICES);
}

bool qEmpty()
{
    bool empty = (head == tail);
    if(empty)
        head = tail = 0;
    return empty;
}

void push(qnode q)
{
    if(!qFull())
    {
        messageQueue[tail].id = q.id;
        messageQueue[tail].type = q.type;
        tail++;
        tail %= MAX_DEVICES;
    }
}

qnode pop()
{
    qnode res;
    if(!qEmpty())
    {
        res.id = messageQueue[head].id;
        res.type = messageQueue[head].type;
        head++;
        head %= MAX_DEVICES;
    }
    return res;
}
