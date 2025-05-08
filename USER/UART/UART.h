#ifndef __UART_H__
#define __UART_H__

#include <stdio.h>
#include "ti_msp_dl_config.h"

void sendData(uint8_t data);
void sendString(char* p);
void sendNum(float num, int decimalPlaces);

#endif