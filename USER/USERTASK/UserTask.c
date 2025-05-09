#include "UserTask.h"

/*
@brief: 轮询扫描当前该执行的任务
*/
void TaskScan(void) {
    switch (TaskMark) {
        case InputImp:
            CalInputImp();
            break;
        case OutputImp:
            CalOutputImp();
            break;
        case Gain:
            CalGain();
            break;
        case AmpFreq:
            PlotAmpFreq();
            break;
        default:
            break;
    }
}

/*
@brief: 轮询扫描按键的状态，判断哪个按键被按下
*/
void BTNScan(void) {
    BTN_getData(&BTNData);
    if(BTNData.left) {
        DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_B_PIN);
        TaskMark = InputImp;    //left按键对应“测量输入阻抗”
    }
    else if(BTNData.right) {
        DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_B_PIN);
        TaskMark = OutputImp;   //right按键对应“测量输出阻抗”
    }
    else if(BTNData.down) {
        DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_B_PIN);
        TaskMark = Gain;        //down按键对应“测量增益”
    }
    else if(BTNData.up) {
        DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_B_PIN);
        TaskMark = AmpFreq;     //up按键对应“绘制幅频曲线”
    }
}

/*
@brief: 测量输入阻抗
*/
void CalInputImp(void) {

}

/*
@brief: 测量输出阻抗
*/
void CalOutputImp(void) {

}

/*
@brief: 测量增益
*/
void CalGain(void) {
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);  //使能ADC中断
    //阻塞执行
    while(TaskMark == Gain) {
        if(gCheckADC) {
            uint16_t OutVol = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_1);
            float OutAmp = AD8310_Map((float)OutVol * 3300 / 4095);
            float Gain = OutAmp / 500;  //DDS输入信号恒定500mVpp
            //发送数据
            sendString("Gain: ");sendNum(Gain, 2);sendString("\r\n");

            gCheckADC = false;
            DL_ADC12_enableConversions(ADC12_0_INST);
            DL_ADC12_startConversion(ADC12_0_INST);
        }
    }
    NVIC_DisableIRQ(ADC12_0_INST_INT_IRQN); //关闭ADC中断
}

/*
@brief: 绘制幅频曲线
*/
void PlotAmpFreq(void) {
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);  //使能ADC中断
    NVIC_EnableIRQ(UART_0_INST_INT_IRQN);   //使能串口接收中断
    while(TaskMark == AmpFreq) {
        if(gCheckADC) {
            
        }

    }
    NVIC_DisableIRQ(UART_0_INST_INT_IRQN);  //关闭串口接收中断
    NVIC_DisableIRQ(ADC12_0_INST_INT_IRQN); //关闭ADC中断
}

/*
@brief: AD8310输出电平 -> 输入信号幅值
*/
float AD8310_Map(float Amp) {
    float res = 0.0;

    return Amp;
}