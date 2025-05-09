#include "ti_msp_dl_config.h"
#include "../USER/UART/UART.h"
#include "../USER/BTN/BTN.h"
#include "../USER/TICK/Tick.h"
#include "../USER/USERTASK/UserTask.h"
#include <string.h>
#include <stdlib.h>

void parse_rx_buffer(char* , float* , float* );

volatile bool gCheckADC = false;
volatile bool uart_rx_finish = false;
volatile char uart_rx_buffer[RX_BUFFER_SIZE];
volatile uint8_t uart_rx_index = 0;

volatile float freq = 0.0;
volatile float mag = 0.0;

BTNData_t BTNData = {0};

volatile uint8_t TaskMark = 0;  //执行任务标志位，初始不执行任何任务

int main(void)
{
    SYSCFG_DL_init();  
	//NVIC_EnableIRQ(UART_0_INST_INT_IRQN);

    while (1)
    {
		if(uart_rx_finish) {
            parse_rx_buffer(uart_rx_buffer, &freq, &mag);
            sendString("freq: ");sendNum(freq, 2); sendString("\t");
            sendString("mag: ");sendNum(mag, 2); sendString("\r\n");

            NVIC_EnableIRQ(UART_0_INST_INT_IRQN);
            uart_rx_finish = false;
        }
    }
}

void UART_0_INST_IRQHandler(void) {
    switch (DL_UART_Main_getPendingInterrupt(UART_0_INST)) {
        case DL_UART_MAIN_IIDX_RX:
            DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_R_PIN);
            uint8_t data = DL_UART_Main_receiveData(UART_0_INST);
            if(data == '\n') {
                uart_rx_index = 0;
            }
            else if(data == '\r') {
                uart_rx_buffer[uart_rx_index] = '\0';
                uart_rx_finish = true;
                NVIC_DisableIRQ(UART_0_INST_INT_IRQN);
            }
            else if(uart_rx_index < RX_BUFFER_SIZE - 1 && !uart_rx_finish) {
                uart_rx_buffer[uart_rx_index++] = data;
            }
            break;
    }
}

void ADC12_0_INST_IRQHandler(void) {
    switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
        case DL_ADC12_IIDX_MEM1_RESULT_LOADED:
            DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_G_PIN);
            gCheckADC = true;
            break;
        default:
            break;
    }
}

/*
@brief: 将串口接收的信号相关信息解包
@param: buffer - 串口缓冲区指针
@param: freq - 频率
@param: mag - 幅度
*/
void parse_rx_buffer(char* buffer, float* freq, float* mag) {
    *freq = strtof(buffer, NULL);
    *mag = strtof(strchr(buffer, ' ') + 1, NULL);
}