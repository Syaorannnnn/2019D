#include "UART.h"

void sendData(uint8_t data) {
    while(DL_UART_isBusy(UART_1_INST));
    DL_UART_Main_transmitData(UART_1_INST, data);
}

void sendString(char *p) {
    while(*p != '\0') {
        sendData(*p);
        p++;
    }
}
/*
@brief: 串口发送数字
@param: num - 要发送的数字
@param: decimalPlaces - 小数位数
*/
void sendNum(float num, int decimalPlaces) {
    char str[20];
    char format[10];
    sprintf(format, "%%.%df", decimalPlaces);
    sprintf(str, format, num);
    sendString(str);
}