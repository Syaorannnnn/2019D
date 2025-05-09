#ifndef __USERTASK_H__
#define __USERTASK_H__

#include "ti_msp_dl_config.h"
#include "../USER/UART/UART.h"
#include "../USER/BTN/BTN.h"
#include "../USER/TICK/Tick.h"
#include <string.h>
#include <stdlib.h>

#define RX_BUFFER_SIZE 30
#define SIGNAL_IN_mVPP  500.f
#define RESISTOR_IN    1000.0f     //信号源与放大器之间串联的电阻大小
#define RESISTOR_LOAD  1000.0f     //放大器负载电路大小

enum {
    InputImp = 1,   //输入阻抗
    OutputImp = 2,  //输出阻抗
    Gain = 3,       //增益
    AmpFreq = 4,    //幅频曲线
};
//测量输出电阻时的状态
enum {
    Open = 1,       //负载开路
    Load = 2,       //负载接入
};

typedef struct Circuit_Paramter {
    float Ri;   //输入电阻
    float Ro;   //输出电阻
    float Av;   //电压增益
    float fh;   //3dB上截止频率
}Circuit_Paramter;

extern volatile uint8_t TaskMark;
extern BTNData_t BTNData;

extern volatile bool uart_rx_finish;
extern volatile char uart_rx_buffer[RX_BUFFER_SIZE];
extern volatile uint8_t uart_rx_index;
extern volatile bool gCheckADC;

extern Circuit_Paramter c_param;

//测量输出阻抗需要的全局变量
extern volatile uint8_t Out_state;
extern volatile float Vout_Load;     //接入负载时的输出电压
extern volatile float Vout_Open;     //负载开路时的输出电压
extern volatile bool Vout_flag;      //是否测量完成开路和负载输出电压的标志位

void TaskScan(void);
void BTNScan(void);
void CalInputImp(void);
void CalOutputImp(void);
void CalGain(void);
void PlotAmpFreq(void);
float AD8310_Map(float Amp);
void parse_rx_buffer(char* , float* , float* );

#endif