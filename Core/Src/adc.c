/*
 * adc.c
 *
 *  Created on: 2025��6��2��
 *      Author: 22934
 */

#include "adc.h"


float V_all = 0.0f; // ��Դ��ѹ

struct Curr_data Curr_threephase = {0.0f, 0.0f, 0.0f}; // �������ݽṹ��

void Adc_Init_Epwm(){
    //��ʼ��ADC
    DINT;                          // �����ж�
    InitAdc();                     // ��ʼ��ADC
    EALLOW;
    // ADC�ο���ѹѡ��
    AdcRegs.ADCCTL1.bit.ADCREFSEL = 0;    // 0���ڲ��ο���ѹ


    // ADC���ƼĴ�������
    AdcRegs.ADCCTL2.bit.CLKDIV2EN = 1;
    AdcRegs.ADCCTL2.bit.CLKDIV4EN = 0;    // 45MHZ adc
    AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;  // ��ת�����н���ʱ�����ж�
    AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 1;// ʹ�ܷ��ص�����

    // ���ò�������
    AdcRegs.ADCSOC0CTL.bit.ACQPS = 15;    // ����Ϊ15��ADCʱ�����ڵĲ�������
    AdcRegs.ADCSOC1CTL.bit.ACQPS = 15;
    AdcRegs.ADCSOC2CTL.bit.ACQPS = 15;
    AdcRegs.ADCSOC3CTL.bit.ACQPS = 15;
    AdcRegs.ADCSOC4CTL.bit.ACQPS = 15;
    AdcRegs.ADCSOC5CTL.bit.ACQPS = 15;
    // ����ת��ͨ���ʹ���Դ
    AdcRegs.ADCSOC0CTL.bit.CHSEL = 0;     // SOC0ѡ��ͨ��A0
    AdcRegs.ADCSOC0CTL.bit.TRIGSEL = 5;   // EPWM1 SOCA����

    AdcRegs.ADCSOC1CTL.bit.CHSEL = 1;     // SOC1ѡ��ͨ��A1
    AdcRegs.ADCSOC1CTL.bit.TRIGSEL = 5;   // EPWM1 SOCA����

    AdcRegs.ADCSOC2CTL.bit.CHSEL = 2;     // SOC2ѡ��ͨ��A2
    AdcRegs.ADCSOC2CTL.bit.TRIGSEL = 5;   // EPWM1 SOCA����

    AdcRegs.ADCSOC3CTL.bit.CHSEL = 3;     // SOC3ѡ��ͨ��A3
    AdcRegs.ADCSOC3CTL.bit.TRIGSEL = 5;   // EPWM1 SOCA����

    AdcRegs.ADCSOC4CTL.bit.CHSEL = 4;     // SOC4ѡ��ͨ��A4
    AdcRegs.ADCSOC4CTL.bit.TRIGSEL = 5;   // EPWM1 SOCA����

    AdcRegs.ADCSOC5CTL.bit.CHSEL = 5;     // SOC5ѡ��ͨ��A5
    AdcRegs.ADCSOC5CTL.bit.TRIGSEL = 5;   // EPWM1 SOCA����

    // ����ADC�ж�
    AdcRegs.INTSEL1N2.bit.INT1SEL = 5;    // SOC5���ʱ����ADCINT1
    AdcRegs.INTSEL1N2.bit.INT1E = 1;      // ʹ��ADCINT1�ж�

    AdcRegs.INTSEL1N2.bit.INT1CONT = 0;   // ��ÿ��ת�������󴥷��ж�

    EDIS;

    // �����ж�������
    EALLOW;
    PieVectTable.ADCINT1 = &Adc_get_data;  // ���жϺ���
    EDIS;

    // ʹ���ж�
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;     // ʹ��ADC�жϣ���F2806x����PIE��1��INT1��
    IER |= M_INT1;                         // ʹ��CPU�ж�1
    EINT;                               // Enable Global interrupt INTM
    ERTM;                               // Enable Global realtime interrupt DBGM

}


interrupt void Adc_get_data(){
    // //ԭʼ��ѹ��������
    // Curr_threephase.Ia = (float)(AdcResult.ADCRESULT1 * 3.3f / 4096.0f - 1.5f) / 20 / 0.02; // A�����
    // Curr_threephase.Ib = (float)(AdcResult.ADCRESULT1 * 3.3f / 4096.0f - 1.5f) / 20 / 0.02; // B�����
    // V_all = (float)AdcResult.ADCRESULT0 * 3.3f / 4096.0f * 39.3f; // ��Դ��ѹ

    //������ʡ�Ժ�ĵ�ѹ��������
    Curr_threephase.Ia = (float)(AdcResult.ADCRESULT1 * 0.0008f - 1.5f) * 2.5f; // A�����
    Curr_threephase.Ib = (float)(AdcResult.ADCRESULT2 * 0.0008f - 1.5f) * 2.5f; // B�����
    V_all = (float)AdcResult.ADCRESULT0 * 0.03144f; // ��Դ��ѹ


    // ����жϱ�־λ
    AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;

    // ȷ���ж��ѱ�����
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1; // ȷ��PIE��1�ж�

}
