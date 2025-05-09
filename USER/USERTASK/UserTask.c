#include "UserTask.h"

/*
@brief: 轮询扫描当前该执行的任务
*/
void TaskScan(void) {
    switch (TaskMark) {
        case InputImp:
            sendString("CalInputImp()\r\n");
            CalInputImp();
            break;
        case OutputImp:
            sendString("CalOutputImp()\r\n");
            CalOutputImp();
            break;
        case Gain:
            sendString("CalGain()\r\n");
            CalGain();
            break;
        case AmpFreq:
            sendString("PlotAmpFreq()\r\n");
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
        //sendString("left pressed\r\n");
        TaskMark = InputImp;    //left按键对应“测量输入阻抗”
    }
    if(BTNData.right) {
        DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_B_PIN);
        //sendString("right pressed\r\n");
        TaskMark = OutputImp;   //right按键对应“测量输出阻抗”
    }
    if(BTNData.down) {
        DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_B_PIN);
        //sendString("down pressed\r\n");
        TaskMark = Gain;        //down按键对应“测量增益”
    }
    if(BTNData.up) {
        DL_GPIO_togglePins(GPIO_LED_PORT, GPIO_LED_B_PIN);
        //sendString("up pressed\r\n");
        TaskMark = AmpFreq;     //up按键对应“绘制幅频曲线”
    }
}

/*
@brief: 测量输入阻抗
*/
void CalInputImp(void) {
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);  //使能ADC中断
    DL_ADC12_startConversion(ADC12_0_INST);
    //阻塞执行
    while(TaskMark == InputImp) {
        if(gCheckADC) {
            uint16_t InVol = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
            float InAmp = AD8310_Map((float)InVol * 3300 / 4095);

            float R_i = InAmp / (SIGNAL_IN_mVPP - InAmp) * RESISTOR_IN;      //通过串联分压计算放大器输入电阻
            c_param.Ri = R_i;
            sendString("Input Resistor: "); sendNum(R_i, 2); sendString("\r\n");

            gCheckADC = false;
            DL_ADC12_enableConversions(ADC12_0_INST);
            DL_ADC12_startConversion(ADC12_0_INST);
        }            
    }
    NVIC_DisableIRQ(ADC12_0_INST_INT_IRQN); //关闭ADC中断
}

/*
@brief: 测量输出阻抗
*/
void CalOutputImp(void) {
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);  //使能ADC中断
    DL_ADC12_startConversion(ADC12_0_INST);
    Out_state = Load;       //初始化为带负载
    Vout_flag = false;
    //阻塞执行
    while(TaskMark == OutputImp) {
        if(Vout_flag) {
            c_param.Ro = (Vout_Open / Vout_Load - 1) * RESISTOR_LOAD;
            sendString("Output Resistor: "); sendNum(c_param.Ro, 2); sendString("\r\n");
            Vout_flag = false;
        }
        if(gCheckADC) {
            if(Out_state == Load) {
                //先测量带负载电压
                uint16_t OutVol = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_1);
                Vout_Load = AD8310_Map((float)OutVol * 3300 / 4095);
                Out_state = Open;
            }
            else if(Out_state == Open) {
                //再测量负载开路电压
                DL_GPIO_clearPins(GPIO_CON_PORT, GPIO_CON_OUT_PIN);     //控制继电器使负载开路
                delay_ms(50);   //稳定时间
                uint16_t OutVol = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
                Vout_Open = AD8310_Map((float)OutVol * 3300 / 4095);
                Vout_flag = true;
            }

            gCheckADC = false;
            DL_ADC12_enableConversions(ADC12_0_INST);
            DL_ADC12_startConversion(ADC12_0_INST);
        }
    }
    DL_GPIO_clearPins(GPIO_CON_PORT, GPIO_CON_OUT_PIN);
    NVIC_DisableIRQ(ADC12_0_INST_INT_IRQN); //关闭ADC中断
}

/*
@brief: 测量增益
*/
void CalGain(void) {
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);  //使能ADC中断
    DL_ADC12_startConversion(ADC12_0_INST);
    //阻塞执行
    while(TaskMark == Gain) {
        if(gCheckADC) {
            uint16_t OutVol = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_1);
            float OutAmp = AD8310_Map((float)OutVol * 3300 / 4095);
            float Gain = OutAmp / SIGNAL_IN_mVPP;  //DDS输入信号恒定500mVpp
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
    //拟合公式: Vout = 491.7170 * log10(Vin) + 930.7041
    res = 491.7170 * log10(Amp) + 930.7041;
    return res;
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