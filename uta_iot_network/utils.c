/*
 * utils.c
 *
 *  Created on: Apr 17, 2021
 *      Author: Sarker Nadir Afridi Azmi
 */

#include <stdint.h>
#include "utils.h"

void copyArray(uint8_t src[], uint8_t dest[], uint8_t size)
{
    uint8_t i = 0;
    for(i = 0; i < size; i++)
    {
        dest[i] = src[i];
    }
}
