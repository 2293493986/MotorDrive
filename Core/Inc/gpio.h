/*
 * gpio.h
 *
 *  Created on: 2025Äê6ÔÂ2ÈÕ
 *      Author: 22934
 */

#ifndef CORE_INC_GPIO_H_
#define CORE_INC_GPIO_H_

#include "F2806x_Device.h"
#include "F2806x_Examples.h"

#define Led1_Toggle GpioDataRegs.GPATOGGLE.bit.GPIO20 = 1
#define Led1_open   GpioDataRegs.GPACLEAR.bit.GPIO20  = 1
#define Led1_close  GpioDataRegs.GPASET.bit.GPIO20    = 1


#define Led2_Toggle GpioDataRegs.GPATOGGLE.bit.GPIO21 = 1
#define Led2_open   GpioDataRegs.GPACLEAR.bit.GPIO21  = 1
#define Led2_close  GpioDataRegs.GPASET.bit.GPIO21    = 1

#define Motor_EN_Open (GpioDataRegs.GPBSET.bit.GPIO42 = 1)
#define Motor_EN_Close (GpioDataRegs.GPBCLEAR.bit.GPIO42 = 1)

void InitLEDGPIO(void);
void InitMotor(void);

#endif /* CORE_INC_GPIO_H_ */
