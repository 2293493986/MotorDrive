/*
 * pwm.c
 *
 *  Created on: 2025年6月2日
 *      Author: 22934
 */

#include "pwm.h"

#define EPWM_TIMER_TBPRD  2250  // Period register 系统时钟为90MHz，下面的程序没进行分频，根据公式90M/（2*TBPRD）,除以9000这样得到的是10kHz
#define EPWM1_START_CMPA  0     // 设置PWM通道A初始占空比
#define EPWM1_START_CMPB  0     // 设置PWM通道B初始占空比
#define DEAD_TIME         180   // 死区时间

void Motor_pwm_init(enum PWM_State PWM_Mode) {
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;  // 在配置前禁用PWM时钟同步
    EDIS;

    Epwm_gpio_init(PWM_Mode);

    // 按顺序初始化PWM，EPWM1作为主时钟源
    Epwm_init(PWM_Mode, 1);// 主PWM
    Epwm_init(PWM_Mode, 2);  // 从PWM
    Epwm_init(PWM_Mode, 3);  // 从PWM

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;  // 启用时钟同步
    EPwm1Regs.TBCTL.bit.SWFSYNC = 1;  // 主模块触发同步
    EDIS;
 }


void Epwm_gpio_init(enum PWM_State PWM_Mode) {
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1; // 复用模式
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;  // 配置成输出

    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1; // 复用模式
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;  // 配置成输出

    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1; // 复用模式
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;  // 配置成输出
    EDIS;

    if(PWM_Mode == H_PWM_L_PWM_mode || PWM_Mode == Foc_mode){
        EALLOW;
        GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1; // 复用模式
        GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;  // 配置成输出

        GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1; // 复用模式
        GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;  // 配置成输出

        GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1; // 复用模式
        GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;  // 配置成输出
        EDIS;
    } else if (PWM_Mode == H_PWM_L_ON_mode) {
        EALLOW;
        GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0; // 普通IO模式
        GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;  // 配置成输出
        GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;  // 使能内部上拉

        GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0; // 普通IO模式
        GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;  // 配置成输出
        GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;  // 使能内部上拉

        GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0; // 普通IO模式
        GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;  // 配置成输出
        GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;  // 使能内部上拉
        EDIS;
    }
 }

void Epwm_init(enum PWM_State PWM_Mode, int epwm_num) {
        volatile struct EPWM_REGS *EPwmRegs;
        switch (epwm_num) {
        case 1:
            EPwmRegs = &EPwm1Regs;
            EALLOW;
            SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1; // 打开PWM时钟
            EDIS;
            break;
        case 2:
            EPwmRegs = &EPwm2Regs;
            EALLOW;
            SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1; // 打开PWM时钟
            EDIS;
            break;
        case 3:
            EPwmRegs = &EPwm3Regs;
            EALLOW;
            SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 1; // 打开PWM时钟
            EDIS;
            break;
        default:
            return; // 无效的PWM编号
    }
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0; // 关闭所有基时钟
    EDIS;

    // Setup TBCLK
    EPwmRegs->TBPRD = EPWM_TIMER_TBPRD - 1; // 设置周期
    //     EPwmRegs->TBPHS.half.TBPHS = 0x0000;    // 设置相位
    EPwmRegs->TBCTL.bit.HSPCLKDIV = TB_DIV1;      // 时钟分频设置
    EPwmRegs->TBCTL.bit.CLKDIV = TB_DIV1;
    EPwmRegs->TBCTR = 0x0000;               // 清除计数器
    // Set Compare values
    EPwmRegs->CMPA.half.CMPA = EPWM1_START_CMPA; // 设置比较A值
    EPwmRegs->CMPB = EPWM1_START_CMPB;           // 设置比较B值

    // Setup counter mode
    EPwmRegs->TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // 计数模式
    if(epwm_num == 1) {
        EPwmRegs->TBCTL.bit.PHSEN = TB_DISABLE;     // 主PWM无需同步
        EPwmRegs->TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; // 计数器为0时发出同步信号
        EPwmRegs->TBCTL.bit.HSPCLKDIV = TB_DIV1;    // 时钟分频设置
        EPwmRegs->TBCTL.bit.CLKDIV = TB_DIV1;
        EPwmRegs->TBPHS.half.TBPHS = 0;
    }
    else{
        // 从PWM的同步设置
        EPwmRegs->TBCTL.bit.PHSEN = TB_ENABLE;      // 使能相位同步
        EPwmRegs->TBCTL.bit.SYNCOSEL = TB_SYNC_IN;  // SYNCO由SYNCI触发，级联至下一模块
        EPwmRegs->TBPHS.half.TBPHS = 0;             // 相位设为0
        EPwmRegs->TBCTR = 0;                        // 初始化计数器为0

        // 确保计数器初始值为0
        EPwmRegs->TBCTR = 0;
    }

    EPwmRegs->TBCTL.bit.PHSDIR = TB_COUNT_UPDOWN;  // 向上计数时同步
    EPwmRegs->TBCTL.bit.FREE_SOFT = 3;   // 自由运行模式

    EPwmRegs->TBCTL.bit.PRDLD = TB_SHADOW;        // 使用影子寄存器

    if (PWM_Mode == H_PWM_L_PWM_mode || PWM_Mode == Foc_mode) {
        EPwmRegs->CMPCTL.bit.SHDWBMODE = CC_SHADOW;
        EPwmRegs->CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    }

    // Set actions
    if (PWM_Mode == Foc_mode) { // FOC模式 互补输出
        EPwmRegs->DBCTL.bit.IN_MODE = DBA_ALL;    // PWM死区配置 配置互补波形
        EPwmRegs->DBCTL.bit.POLSEL = DB_ACTV_LOC; // 高电平有效
        EPwmRegs->DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // 双边延时
        EPwmRegs->DBRED = DEAD_TIME;  // 配置上升沿死区时间
        EPwmRegs->DBFED = DEAD_TIME;  // 配置下降沿死区时间
        //         EPwmRegs->AQCTLA.bit.CAU = AQ_CLEAR;
        //         EPwmRegs->AQCTLA.bit.CAD = AQ_SET;
        //         EPwmRegs->AQCTLB.bit.CBU = AQ_SET;
        //         EPwmRegs->AQCTLB.bit.CBD = AQ_CLEAR;

        EPwmRegs->AQCTLA.bit.CAU = AQ_SET;
        EPwmRegs->AQCTLA.bit.CAD = AQ_CLEAR;
        EPwmRegs->AQCTLB.bit.CBU = AQ_CLEAR;
        EPwmRegs->AQCTLB.bit.CBD = AQ_SET;

    } else if (PWM_Mode == H_PWM_L_PWM_mode) { // H_PWM_L_PWM_mode模式，单独输出
        EPwmRegs->AQCTLA.bit.CAU = AQ_SET;
        EPwmRegs->AQCTLA.bit.CAD = AQ_CLEAR;
        EPwmRegs->AQCTLB.bit.CBU = AQ_SET;
        EPwmRegs->AQCTLB.bit.CBD = AQ_CLEAR;
    } else if (PWM_Mode == H_PWM_L_ON_mode) {
        EPwmRegs->AQCTLA.bit.CAU = AQ_SET;   // Set PWM1A on event A, up count
        EPwmRegs->AQCTLA.bit.CAD = AQ_CLEAR; // Clear PWM1A on event A, down count
    }

    EPwm1Regs.ETSEL.bit.SOCAEN = 1;        // 使能ePWMa触发AD转换
    EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_PRD; // 计数到2250进行触发
    EPwm1Regs.ETPS.bit.SOCAPRD = ET_3RD;   // 在第一个触发事件后即产生触发脉冲

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1; // Start all the timers synced
    EDIS;
 }


