/**
 * @file DSP.c
 * @author Aiden
 * @brief DSP模块
 * @version 0.1
 * @date 2025-05-18
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "DSP.h"

/**
 * @brief 滑动均值滤波器
 * 
 * @param #define MVF_LENTH 滤波器长度
 * @param data 
 * @return float 
 */
 float moving_mean_filter(float data) 
 {
    //返回结果
    float result = 0;
    //求和
    float sum = 0;
    //静态变量，标记数据传入的个数
    static int data_num = 0;
    static float buffer[MVF_LENTH] = {0};

    if(data_num < MVF_LENTH)
    {
        buffer[data_num++] = data;
        result = data;
    }
    else
    {
        //buffer[1]之后的数据向左移动一位
        memcpy(&buffer[0],&buffer[1],(MVF_LENTH - 1) * 4);
        //在尾部加入新的data
        buffer[MVF_LENTH - 1] = data;
        //计算平均值
        for(int i = 0; i < MVF_LENTH; i++)
        {
            sum += buffer[i];
        }
        //计算均值
        result = sum / MVF_LENTH;
    }

    return result;
 }

 /**
  * @brief 三点中值滤波
  * 
  * @param data 
  * @return float 
  */
 float median_filter(float data)
 {
    static float buffer[MEDIAN_LENTH] = {0};
    //静态变量，标记数据传入的个数
    static int data_num = 0;
    //数据个数大于等于3，标记为满
    
    //缓冲区未满，数据只进不出
    if(data_num < MEDIAN_LENTH)
    {
        buffer[data_num++] = data;
        return data;
    }
    //缓冲区已满，数据尾进头出
    else
    {
        //buffer[1]之后的数据向左移动一位，一个float 4Byte
        memcpy(&buffer[0],&buffer[1],(MEDIAN_LENTH - 1) * 4);
        //在尾部加入新的data
        buffer[MEDIAN_LENTH - 1] = data;

        //数据区满时才排序
        //冒泡排序（升序）
        for(int i = 0; i < MEDIAN_LENTH - 1; i++)
        {
            for(int j = 0; j < MEDIAN_LENTH - i - 1; j++)
            {
                if(buffer[j] > buffer[j + 1])
                {
                    float temp = buffer[j];
                    buffer[j] = buffer[j + 1];
                    buffer[j + 1] = temp;
                }
            }
        }

        return buffer[MEDIAN_LENTH / 2]; //返回中间值
    }
    
    
 }