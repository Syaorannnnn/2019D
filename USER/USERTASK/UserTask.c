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

}

/*
@brief: 绘制幅频曲线
*/
void PlotAmpFreq(void) {
    
}