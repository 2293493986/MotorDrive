/*
 * timer.c
 *
 *  Created on: 2025��6��2��
 *      Author: 22934
 */

#include "timer.h"
#include "foc.h"

unsigned int ms_1000 = 0;

void Time0_Init(int Freq, int Period){
    EALLOW;  //�ر�д����
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer 0
    //��ʼ����ʱ������-> ����InitCpuTimers();ֱ�����
    //ָ��Timer0�ļĴ�����ַ
    CpuTimer0.RegsAddr=&CpuTimer0Regs;
    //���ڼĴ���ֵ������
    CpuTimer0Regs.PRD.all=0xFFFFFFFF;
    //����Timer0��Ԥ�������ֵΪ0
    CpuTimer0Regs.TPR.all =0;
    CpuTimer0Regs.TPRH.all=0;
    //����Timer0��״̬Ϊֹͣ״̬
    CpuTimer0Regs.TCR.bit.TSS =1;
    //��װ��ʹ��
    CpuTimer0Regs.TCR.bit.TRB =1;
    //�����жϼ�����
    CpuTimer0.InterruptCount =0;
    //��ʱ������
    ConfigCpuTimer(&CpuTimer0,Freq,Period);
    //������ʱ���жϹ��ܲ�ʹ�ܶ�ʱ��
    //����Timer0���ж���ڵ�ַΪ�ж��������INT0
    PieVectTable.TINT0=&TIM0_IRQ;
    //������ʱ������
    CpuTimer0Regs.TCR.bit.TSS=0;
    //����CPU��һ���жϲ�ʹ�ܵ�һ���жϵĵ�7��С�жϣ���ΪTimer0->INT1.7
    IER |= M_INT1;  //ʹ��CPU�ж�1��INT1��
    PieCtrlRegs.PIEIER1.bit.INTx7=1;
    //ʹ�����ж�
    EINT;
    ERTM;   //��ʹ�÷���������ʱ�ɿ��� DEBUG �жϣ���ʹ�� ERTM ���

    EDIS;   //����д����
}

interrupt void TIM0_IRQ(void)
{
    EALLOW;
    ms_1000++;
//    VF_OpenLoop_Control();
    IF_OpenLoop_Control(1000, 5,0.35f);
//    IF_OpenLoop_Control(200, 5,0.1f);
    CpuTimer0Regs.TCR.bit.TIF = 1;//������輰�жϱ�־λ
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;//���PIE���ж�Ӧ��
    EDIS;
}



