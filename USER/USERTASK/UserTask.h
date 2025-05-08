#ifndef __USERTASK_H__
#define __USERTASK_H__

#include "ti_msp_dl_config.h"
#include "../USER/UART/UART.h"
#include "../USER/BTN/BTN.h"
#include "../USER/TICK/Tick.h"
#include <string.h>
#include <stdlib.h>

enum {
    InputImp = 1,   //输入阻抗
    OutputImp = 2,  //输出阻抗
    Gain = 3,       //增益
    AmpFreq = 4,    //幅频曲线
};

extern volatile uint8_t TaskMark;
extern BTNData_t BTNData;

void TaskScan(void);
void BTNScan(void);
void CalInputImp(void);
void CalOutputImp(void);
void CalGain(void);
void PlotAmpFreq(void);

#endif