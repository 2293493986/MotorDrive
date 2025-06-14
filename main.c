/*
    * @file main.c
    * @brief Main program file for F2806x motor control application.
    * @date 2025-06-02
    * 
*/


#include "DSP28x_Project.h"
#include "F2806x_Device.h"      // F2806x Headerfile Include File
#include "F2806x_Examples.h"

#include "timer.h"
#include "pwm.h"
#include "foc.h"
#include "adc.h"
#include "gpio.h"

void main(void)
{
    InitSysCtrl();                         //关闭看门狗 打开PLL
    InitPieCtrl();
    InitPieVectTable();
    Time0_Init(90,400-1);
    InitLEDGPIO();
    InitMotor();
    Adc_Init_Epwm();
    Motor_pwm_init(Foc_mode);
    PID_Load();
    Led2_close;
    Motor_EN_Open;
    while(1){
        if(ms_1000 >= 2500){
            ms_1000 = 0;
            Led1_Toggle;
        }
    }
}



