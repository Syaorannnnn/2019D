#ifndef __TICK_H__
#define __TICK_H__

#include <stdint.h>
#include "ti_msp_dl_config.h"
#include "../BTN/BTN.h"

extern volatile uint32_t Tick;

void delay_ms(uint8_t ms);
#endif /* #ifndef __TICK_H__ */