#ifndef __USERTASK_H__
#define __USERTASK_H__

#include "ti_msp_dl_config.h"
#include "../USER/UART/UART.h"
#include "../USER/BTN/BTN.h"
#include "../USER/TICK/Tick.h"
#include <string.h>
#include <stdlib.h>

#define RX_BUFFER_SIZE 30

enum {
    InputImp = 1,   //输入阻抗
    OutputImp = 2,  //输出阻抗
    Gain = 3,       //增益
    AmpFreq = 4,    //幅频曲线
};

extern volatile uint8_t TaskMark;
extern BTNData_t BTNData;

extern volatile bool uart_rx_finish;
extern volatile char uart_rx_buffer[RX_BUFFER_SIZE];
extern volatile uint8_t uart_rx_index;
extern volatile uint8_t gCheckADC;

void TaskScan(void);
void BTNScan(void);
void CalInputImp(void);
void CalOutputImp(void);
void CalGain(void);
void PlotAmpFreq(void);
float AD8310_Map(float Amp);

#endif