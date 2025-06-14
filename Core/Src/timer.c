/*
 * timer.c
 *
 *  Created on: 2025年6月2日
 *      Author: 22934
 */

#include "timer.h"
#include "foc.h"

unsigned int ms_1000 = 0;

void Time0_Init(int Freq, int Period){
    EALLOW;  //关闭写保护
    SysCtrlRegs.PCLKCR3.bit.CPUTIMER0ENCLK = 1; // CPU Timer 0
    //初始化定时器参数-> 可用InitCpuTimers();直接替代
    //指向Timer0的寄存器地址
    CpuTimer0.RegsAddr=&CpuTimer0Regs;
    //周期寄存器值得配置
    CpuTimer0Regs.PRD.all=0xFFFFFFFF;
    //设置Timer0的预标计数器值为0
    CpuTimer0Regs.TPR.all =0;
    CpuTimer0Regs.TPRH.all=0;
    //设置Timer0的状态为停止状态
    CpuTimer0Regs.TCR.bit.TSS =1;
    //重装载使能
    CpuTimer0Regs.TCR.bit.TRB =1;
    //重置中断计数器
    CpuTimer0.InterruptCount =0;
    //定时器配置
    ConfigCpuTimer(&CpuTimer0,Freq,Period);
    //开启定时器中断功能并使能定时器
    //设置Timer0的中断入口地址为中断向量表的INT0
    PieVectTable.TINT0=&TIM0_IRQ;
    //开启定时器功能
    CpuTimer0Regs.TCR.bit.TSS=0;
    //开启CPU第一组中断并使能第一组中断的第7个小中断，即为Timer0->INT1.7
    IER |= M_INT1;  //使能CPU中断1（INT1）
    PieCtrlRegs.PIEIER1.bit.INTx7=1;
    //使能总中断
    EINT;
    ERTM;   //当使用仿真器调试时可开启 DEBUG 中断，即使用 ERTM 语句

    EDIS;   //开启写保护
}

interrupt void TIM0_IRQ(void)
{
    EALLOW;
    ms_1000++;
//    VF_OpenLoop_Control();
    IF_OpenLoop_Control(1000, 5,0.35f);
//    IF_OpenLoop_Control(200, 5,0.1f);
    CpuTimer0Regs.TCR.bit.TIF = 1;//清除外设及中断标志位
    PieCtrlRegs.PIEACK.bit.ACK1 = 1;//清除PIE级中断应答
    EDIS;
}



