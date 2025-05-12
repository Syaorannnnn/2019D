#include "UART.h"

void sendData(uint8_t data, UART_Regs *huart){
    while(DL_UART_isBusy(huart));
    DL_UART_Main_transmitData(huart, data);
}

void sendString(char *p, UART_Regs *huart) {
    while(*p != '\0') {
        sendData(*p, huart);
        p++;
    }
}

/**
@brief: 串口发送数字
@param: num - 要发送的数字
@param: decimalPlaces - 小数位数
*/
void sendNum(float num, int decimalPlaces, UART_Regs *huart) {
    char str[20];
    char format[10];
    sprintf(format, "%%.%df", decimalPlaces);
    sprintf(str, format, num);
    sendString(str,huart);
}