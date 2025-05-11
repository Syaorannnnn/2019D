#include "UserTask.h"

/*
@brief: 轮询扫描当前该执行的任务
*/
void TaskScan(void) {
    switch (TaskMark) {
        case InputImp:
            //sendString("CalInputImp()\r\n");
            CalInputImp();
            break;
        case OutputImp:
            //sendString("CalOutputImp()\r\n");
            CalOutputImp();
            break;
        case Gain:
            //sendString("CalGain()\r\n");
            CalGain();
            break;
        case AmpFreq:
            //sendString("PlotAmpFreq()\r\n");
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
    switch (InputImpState) {
        case IDLE:
            NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);  //使能ADC中断
            DL_ADC12_enableConversions(ADC12_0_INST);
            DL_ADC12_startConversion(ADC12_0_INST);
            InputImpState = WAIT;
            break;
        case WAIT:
            if(gCheckADC) {
                InputImpState = PROCESS;
            }
            break;
        case PROCESS:
            uint16_t InVol = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);
            //sendNum(InVol, 2);  sendString("\r\n");
            float InAmp = AD8310_Map((float)InVol * 3300 / 4095);
            float R_i = InAmp / (SIGNAL_IN_mVPP - InAmp) * RESISTOR_IN;
            R_i = 1 / (1 / R_i - 1 / RESISTOR_BING);
            c_param.Ri = R_i;
            sendString("Input Resistor: "); sendNum(R_i, 2); sendString("\r\n");

            gCheckADC = false;
            InputImpState = IDLE;
            break;
    }
    //按键打断当前任务
    if(TaskMark != InputImp) {
        InputImpState = IDLE;
        NVIC_DisableIRQ(ADC12_0_INST_INT_IRQN);
    }
}

/*
@brief: 测量输出阻抗
*/
void CalOutputImp(void) {
    switch (OutputImpState) {
        case IDLE:
            NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);  //使能ADC中断
            DL_GPIO_clearPins(GPIO_CON_PORT, GPIO_CON_OUT_PIN);
            DL_Common_delayCycles(32000000);    //延时1s
            DL_ADC12_enableConversions(ADC12_0_INST);
            DL_ADC12_startConversion(ADC12_0_INST);
            Out_state = Open;       //先测量开路的输出电压
            Vout_flag = false;
            OutputImpState = WAIT;
            break;
        case WAIT:
            if(gCheckADC) {
                OutputImpState = PROCESS;
            }
            break;
        case PROCESS:
            if(Out_state == Open) {
                uint16_t OutVol = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_1);
                float vout_open = AD8310_Map((float)OutVol * 3300 / 4095);
                sendString("Vout_Open: "); sendNum(vout_open, 2); sendString("\r\n");
                
                count_open++;
                if(count_open >= ADC_NUM - 1) {
                    Vout_Open = 0.0;
                    for(int i = 0; i < ADC_NUM; i++) {
                        Vout_Open += Vout_Open_buffer[i];
                    }
                    Vout_Open /= ADC_NUM;
                    vout_index = 0;
                    Out_state = Load;
                    DL_GPIO_setPins(GPIO_CON_PORT, GPIO_CON_OUT_PIN);   //吸合继电器 -> 连接负载
                    DL_Common_delayCycles(32000000);    //延时1s
                    count_open = 0;
                }
                else {
                    Vout_Open_buffer[vout_index++] = vout_open;
                }

                DL_ADC12_enableConversions(ADC12_0_INST);
                DL_ADC12_startConversion(ADC12_0_INST);
                gCheckADC = false;
                OutputImpState = WAIT;
            }
            else if(Out_state == Load) {
                uint16_t OutVol = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_1);
                float vout_load = AD8310_Map((float)OutVol * 3300 / 4095);
                sendString("Vout_Load: "); sendNum(vout_load, 2); sendString("\r\n");

                count_load++;
                if(count_load >= ADC_NUM - 1) {
                    Vout_Load = 0.0;
                    for(int i = 0; i < ADC_NUM; i++) {
                        Vout_Load += Vout_Load_buffer[i];
                    }
                    Vout_Load /= ADC_NUM;
                    vout_index = 0;
                    count_load = 0;
                    float R_o = (Vout_Open / Vout_Load - 1.0f) * RESISTOR_LOAD;
                    c_param.Ro = R_o;
                    sendString("Output Resistor: "); sendNum(R_o, 2); sendString("\r\n");
                    OutputImpState = IDLE;
                    gCheckADC = false;
                }
                else {
                    Vout_Load_buffer[vout_index++] = vout_load;
                    gCheckADC = false;
                    OutputImpState = WAIT;
                    DL_ADC12_enableConversions(ADC12_0_INST);
                    DL_ADC12_startConversion(ADC12_0_INST);
                }
            }
            break;
    }
    if(TaskMark != OutputImp) {
        OutputImpState = IDLE;
        NVIC_DisableIRQ(ADC12_0_INST_INT_IRQN);
        DL_GPIO_clearPins(GPIO_CON_PORT, GPIO_CON_OUT_PIN);   //断开负载
        DL_Common_delayCycles(32000000);    //延时1s
    }
}

/*
@brief: 测量增益
*/
void CalGain(void) {
    switch (GainState) {
        case IDLE:
            NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);  //使能ADC中断
            DL_ADC12_enableConversions(ADC12_0_INST);
            DL_ADC12_startConversion(ADC12_0_INST);
            DL_GPIO_clearPins(GPIO_CON_PORT, GPIO_CON_OUT_PIN);   //断开负载
            DL_Common_delayCycles(32000000);    //延时1s
            GainState = WAIT;
            break;
        case WAIT:
            if(gCheckADC) {
                GainState = PROCESS;
            }
            break;
        case PROCESS:
            uint16_t OutVol = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_1);
            float OutAmp = AD8310_Map((float)OutVol * 3300 / 4095);
            float Gain = OutAmp / SIGNAL_IN_mVPP;
            c_param.Av = Gain;

            sendString("Gain: "); sendNum(Gain, 2); sendString("\r\n");

            gCheckADC = false;
            GainState = IDLE;
            NVIC_DisableIRQ(ADC12_0_INST_INT_IRQN);
            break;
    }
    if(TaskMark != Gain) {
        GainState = IDLE;
        NVIC_DisableIRQ(ADC12_0_INST_INT_IRQN);
    }
}

/*
@brief: 绘制幅频曲线
*/
void PlotAmpFreq(void) {
    switch (PlotState) {
        case WAIT_COMMAND:
            NVIC_EnableIRQ(UART_0_INST_INT_IRQN);   //开启串口中断
            NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);  //使能ADC中断
            if(uart_rx_finish) {
                parse_rx_buffer((char *)uart_rx_buffer, &input_freq, &input_amp);
                uart_rx_finish = false;
                uart_rx_index = 0;
                NVIC_DisableIRQ(UART_0_INST_INT_IRQN);  //关闭中断直到采样完成
                PlotState = START_ADC;
            }
            break;
        case START_ADC:
            DL_ADC12_enableConversions(ADC12_0_INST);
            DL_ADC12_startConversion(ADC12_0_INST);
            PlotState = WAIT_ADC;
            break;
        case WAIT_ADC:
            if(gCheckADC) {
                gCheckADC = false;
                PlotState = DRAW_POINT;
            }
            break;
        case DRAW_POINT:
            uint16_t OutVol = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_1);
            float OutAmp = AD8310_Map((float)OutVol * 3300 / 4095);
            float gain = OutAmp / input_amp;
            if(point_index < MAX_POINTS) {
                freq_buffer[point_index] = input_freq;
                gain_buffer[point_index] = gain;
                point_index++;
            }
            uint16_t x = mapFreqToX(input_freq);
            uint16_t y = mapGainToY(gain);
            
            if(lastX >= 0) {
                //画线
            }
            else {
                //画线
            }
            lastX = x; lastY = y;
            PlotState = WAIT_COMMAND;
            break;
    }
    if(TaskMark != AmpFreq) {
        PlotState = WAIT_COMMAND;
        point_index = 0;
        lastX = lastY = -1;
        NVIC_DisableIRQ(UART_0_INST_INT_IRQN);
        NVIC_DisableIRQ(ADC12_0_INST_INT_IRQN);
    }
}

/*
@brief: AD8310输出电平 -> 输入信号幅值
*/
float AD8310_Map(float Amp) {
    float res = 0.0;
    //拟合公式: Vout = 491.7170 * log10(Vin) + 930.7041
    //res = 491.7170 * log10(Amp) + 930.7041;
    res = pow(10, (Amp - 930.7041) / 491.7170);
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

/*
@brief: 将频率映射到显示屏的X坐标
*/
uint16_t mapFreqToX(float freq) {
    float logF = log10f(freq);
    float logStart = log10f(FREQ_START);
    float logEnd = log10f(FREQ_END);
    uint16_t res = (uint16_t)(((logF - logStart) / (logEnd - logStart)) * 1 /*此处应为显示屏的宽度*/);
    return res;
}

/*
@brief: 将增益映射到显示屏的Y坐标
*/
uint16_t mapGainToY(float gain) {
    if(gain > GAIN_MAX) gain = GAIN_MAX;
    if(gain < 0) gain = 0;
    uint16_t res = (uint16_t)((1.0f - gain / GAIN_MAX) * 1 /*此处应为显示屏的高度*/);
    return res;
}