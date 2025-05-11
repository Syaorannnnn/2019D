#ifndef __UART_H__
#define __UART_H__

#include <stdio.h>
#include "ti_msp_dl_config.h"

void sendData(uint8_t data, UART_Regs *huart);
void sendString(char *p, UART_Regs *huart);
void sendNum(float num, int decimalPlaces, UART_Regs *huart);

#endif