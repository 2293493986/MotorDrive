/*
 * adc.c
 *
 *  Created on: 2025年6月2日
 *      Author: 22934
 */

#include "adc.h"


float V_all = 0.0f; // 电源电压

struct Curr_data Curr_threephase = {0.0f, 0.0f, 0.0f}; // 电流数据结构体

void Adc_Init_Epwm(){
    //初始化ADC
    DINT;                          // 禁用中断
    InitAdc();                     // 初始化ADC
    EALLOW;
    // ADC参考电压选择
    AdcRegs.ADCCTL1.bit.ADCREFSEL = 0;    // 0：内部参考电压


    // ADC控制寄存器配置
    AdcRegs.ADCCTL2.bit.CLKDIV2EN = 1;
    AdcRegs.ADCCTL2.bit.CLKDIV4EN = 0;    // 45MHZ adc
    AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;  // 在转换序列结束时发出中断
    AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 1;// 使能非重叠采样

    // 设置采样窗口
    AdcRegs.ADCSOC0CTL.bit.ACQPS = 15;    // 设置为15个ADC时钟周期的采样窗口
    AdcRegs.ADCSOC1CTL.bit.ACQPS = 15;
    AdcRegs.ADCSOC2CTL.bit.ACQPS = 15;
    AdcRegs.ADCSOC3CTL.bit.ACQPS = 15;
    AdcRegs.ADCSOC4CTL.bit.ACQPS = 15;
    AdcRegs.ADCSOC5CTL.bit.ACQPS = 15;
    // 配置转换通道和触发源
    AdcRegs.ADCSOC0CTL.bit.CHSEL = 0;     // SOC0选择通道A0
    AdcRegs.ADCSOC0CTL.bit.TRIGSEL = 5;   // EPWM1 SOCA触发

    AdcRegs.ADCSOC1CTL.bit.CHSEL = 1;     // SOC1选择通道A1
    AdcRegs.ADCSOC1CTL.bit.TRIGSEL = 5;   // EPWM1 SOCA触发

    AdcRegs.ADCSOC2CTL.bit.CHSEL = 2;     // SOC2选择通道A2
    AdcRegs.ADCSOC2CTL.bit.TRIGSEL = 5;   // EPWM1 SOCA触发

    AdcRegs.ADCSOC3CTL.bit.CHSEL = 3;     // SOC3选择通道A3
    AdcRegs.ADCSOC3CTL.bit.TRIGSEL = 5;   // EPWM1 SOCA触发

    AdcRegs.ADCSOC4CTL.bit.CHSEL = 4;     // SOC4选择通道A4
    AdcRegs.ADCSOC4CTL.bit.TRIGSEL = 5;   // EPWM1 SOCA触发

    AdcRegs.ADCSOC5CTL.bit.CHSEL = 5;     // SOC5选择通道A5
    AdcRegs.ADCSOC5CTL.bit.TRIGSEL = 5;   // EPWM1 SOCA触发

    // 配置ADC中断
    AdcRegs.INTSEL1N2.bit.INT1SEL = 5;    // SOC5完成时触发ADCINT1
    AdcRegs.INTSEL1N2.bit.INT1E = 1;      // 使能ADCINT1中断

    AdcRegs.INTSEL1N2.bit.INT1CONT = 0;   // 在每个转换结束后触发中断

    EDIS;

    // 配置中断向量表
    EALLOW;
    PieVectTable.ADCINT1 = &Adc_get_data;  // 绑定中断函数
    EDIS;

    // 使能中断
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;     // 使能ADC中断（在F2806x中是PIE组1的INT1）
    IER |= M_INT1;                         // 使能CPU中断1
    EINT;                               // Enable Global interrupt INTM
    ERTM;                               // Enable Global realtime interrupt DBGM

}


interrupt void Adc_get_data(){
    // //原始电压电流数据
    // Curr_threephase.Ia = (float)(AdcResult.ADCRESULT1 * 3.3f / 4096.0f - 1.5f) / 20 / 0.02; // A相电流
    // Curr_threephase.Ib = (float)(AdcResult.ADCRESULT1 * 3.3f / 4096.0f - 1.5f) / 20 / 0.02; // B相电流
    // V_all = (float)AdcResult.ADCRESULT0 * 3.3f / 4096.0f * 39.3f; // 电源电压

    //将计算省略后的电压电流计算
    Curr_threephase.Ia = (float)(AdcResult.ADCRESULT1 * 0.0008f - 1.5f) * 2.5f; // A相电流
    Curr_threephase.Ib = (float)(AdcResult.ADCRESULT2 * 0.0008f - 1.5f) * 2.5f; // B相电流
    V_all = (float)AdcResult.ADCRESULT0 * 0.03144f; // 电源电压


    // 清除中断标志位
    AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

    // 确认中断已被处理
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; // 确认PIE组1中断

}
