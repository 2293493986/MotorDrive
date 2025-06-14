/*
 * timer.h
 *
 *  Created on: 2025Äê6ÔÂ2ÈÕ
 *      Author: 22934
 */

#ifndef CORE_INC_TIMER_H_
#define CORE_INC_TIMER_H_

#include "F2806x_Device.h"
#include "F2806x_Examples.h"

interrupt void TIM0_IRQ(void);
void Time0_Init(int Freq, int Period);

extern unsigned int ms_1000;

#endif /* CORE_INC_TIMER_H_ */
