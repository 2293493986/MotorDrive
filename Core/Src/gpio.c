/*
 * gpio.c
 *
 *  Created on: 2025��6��2��
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
    GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 0;   // ʹ��Ϊͨ��IO��
    GpioCtrlRegs.GPBDIR.bit.GPIO42 = 1;  // ����Ϊ���ģʽ

    GpioCtrlRegs.GPADIR.bit.GPIO23 = 0;  // ʹ��Ϊͨ��IO��
    GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 0;   // ����Ϊ����ģʽ
    EDIS;
}

