/*
 * gpio.c
 *
 *  Created on: 2025年6月2日
 *      Author: 22934
 */

#include "gpio.h"

void InitLEDGPIO(void){
    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO20 = 1;

    GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 0;
    GpioCtrlRegs.GPADIR.bit.GPIO21 = 1;
    EDIS;
}

void InitMotor(void){
    EALLOW;
    GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 0;   // 使能为通用IO口
    GpioCtrlRegs.GPBDIR.bit.GPIO42 = 1;  // 设置为输出模式

    GpioCtrlRegs.GPADIR.bit.GPIO23 = 0;  // 使能为通用IO口
    GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 0;   // 设置为输入模式
    EDIS;
}

