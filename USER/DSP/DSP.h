/*

*/

#ifndef __DSP_H__
#define __DSP_H__

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "ti_msp_dl_config.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MVF_LENTH 8     //滑动均值滤波器长度
#define MEDIAN_LENTH 3 //中值滤波器长度

/***********************滤波器*****************************/
float moving_mean_filter(float data);
float median_filter(float data);

#ifdef __cplusplus
}
#endif

#endif