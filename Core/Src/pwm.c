/*
 * pwm.c
 *
 *  Created on: 2025��6��2��
 *      Author: 22934
 */

#include "pwm.h"

#define EPWM_TIMER_TBPRD  2250  // Period register ϵͳʱ��Ϊ90MHz������ĳ���û���з�Ƶ�����ݹ�ʽ90M/��2*TBPRD��,����9000�����õ�����10kHz
#define EPWM1_START_CMPA  0     // ����PWMͨ��A��ʼռ�ձ�
#define EPWM1_START_CMPB  0     // ����PWMͨ��B��ʼռ�ձ�
#define DEAD_TIME         180   // ����ʱ��

void Motor_pwm_init(enum PWM_State PWM_Mode) {
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;  // ������ǰ����PWMʱ��ͬ��
    EDIS;

    Epwm_gpio_init(PWM_Mode);

    // ��˳���ʼ��PWM��EPWM1��Ϊ��ʱ��Դ
    Epwm_init(PWM_Mode, 1);// ��PWM
    Epwm_init(PWM_Mode, 2);  // ��PWM
    Epwm_init(PWM_Mode, 3);  // ��PWM

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;  // ����ʱ��ͬ��
    EPwm1Regs.TBCTL.bit.SWFSYNC = 1;  // ��ģ�鴥��ͬ��
    EDIS;
 }


void Epwm_gpio_init(enum PWM_State PWM_Mode) {
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 1; // ����ģʽ
    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;  // ���ó����

    GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1; // ����ģʽ
    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;  // ���ó����

    GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1; // ����ģʽ
    GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;  // ���ó����
    EDIS;

    if(PWM_Mode == H_PWM_L_PWM_mode || PWM_Mode == Foc_mode){
        EALLOW;
        GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1; // ����ģʽ
        GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;  // ���ó����

        GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 1; // ����ģʽ
        GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;  // ���ó����

        GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 1; // ����ģʽ
        GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;  // ���ó����
        EDIS;
    } else if (PWM_Mode == H_PWM_L_ON_mode) {
        EALLOW;
        GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0; // ��ͨIOģʽ
        GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;  // ���ó����
        GpioCtrlRegs.GPAPUD.bit.GPIO1 = 0;  // ʹ���ڲ�����

        GpioCtrlRegs.GPAMUX1.bit.GPIO3 = 0; // ��ͨIOģʽ
        GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;  // ���ó����
        GpioCtrlRegs.GPAPUD.bit.GPIO3 = 0;  // ʹ���ڲ�����

        GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0; // ��ͨIOģʽ
        GpioCtrlRegs.GPADIR.bit.GPIO5 = 1;  // ���ó����
        GpioCtrlRegs.GPAPUD.bit.GPIO5 = 0;  // ʹ���ڲ�����
        EDIS;
    }
 }

void Epwm_init(enum PWM_State PWM_Mode, int epwm_num) {
        volatile struct EPWM_REGS *EPwmRegs;
        switch (epwm_num) {
        case 1:
            EPwmRegs = &EPwm1Regs;
            EALLOW;
            SysCtrlRegs.PCLKCR1.bit.EPWM1ENCLK = 1; // ��PWMʱ��
            EDIS;
            break;
        case 2:
            EPwmRegs = &EPwm2Regs;
            EALLOW;
            SysCtrlRegs.PCLKCR1.bit.EPWM2ENCLK = 1; // ��PWMʱ��
            EDIS;
            break;
        case 3:
            EPwmRegs = &EPwm3Regs;
            EALLOW;
            SysCtrlRegs.PCLKCR1.bit.EPWM3ENCLK = 1; // ��PWMʱ��
            EDIS;
            break;
        default:
            return; // ��Ч��PWM���
    }
    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0; // �ر����л�ʱ��
    EDIS;

    // Setup TBCLK
    EPwmRegs->TBPRD = EPWM_TIMER_TBPRD - 1; // ��������
    //     EPwmRegs->TBPHS.half.TBPHS = 0x0000;    // ������λ
    EPwmRegs->TBCTL.bit.HSPCLKDIV = TB_DIV1;      // ʱ�ӷ�Ƶ����
    EPwmRegs->TBCTL.bit.CLKDIV = TB_DIV1;
    EPwmRegs->TBCTR = 0x0000;               // ���������
    // Set Compare values
    EPwmRegs->CMPA.half.CMPA = EPWM1_START_CMPA; // ���ñȽ�Aֵ
    EPwmRegs->CMPB = EPWM1_START_CMPB;           // ���ñȽ�Bֵ

    // Setup counter mode
    EPwmRegs->TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // ����ģʽ
    if(epwm_num == 1) {
        EPwmRegs->TBCTL.bit.PHSEN = TB_DISABLE;     // ��PWM����ͬ��
        EPwmRegs->TBCTL.bit.SYNCOSEL = TB_CTR_ZERO; // ������Ϊ0ʱ����ͬ���ź�
        EPwmRegs->TBCTL.bit.HSPCLKDIV = TB_DIV1;    // ʱ�ӷ�Ƶ����
        EPwmRegs->TBCTL.bit.CLKDIV = TB_DIV1;
        EPwmRegs->TBPHS.half.TBPHS = 0;
    }
    else{
        // ��PWM��ͬ������
        EPwmRegs->TBCTL.bit.PHSEN = TB_ENABLE;      // ʹ����λͬ��
        EPwmRegs->TBCTL.bit.SYNCOSEL = TB_SYNC_IN;  // SYNCO��SYNCI��������������һģ��
        EPwmRegs->TBPHS.half.TBPHS = 0;             // ��λ��Ϊ0
        EPwmRegs->TBCTR = 0;                        // ��ʼ��������Ϊ0

        // ȷ����������ʼֵΪ0
        EPwmRegs->TBCTR = 0;
    }

    EPwmRegs->TBCTL.bit.PHSDIR = TB_COUNT_UPDOWN;  // ���ϼ���ʱͬ��
    EPwmRegs->TBCTL.bit.FREE_SOFT = 3;   // ��������ģʽ

    EPwmRegs->TBCTL.bit.PRDLD = TB_SHADOW;        // ʹ��Ӱ�ӼĴ���

    if (PWM_Mode == H_PWM_L_PWM_mode || PWM_Mode == Foc_mode) {
        EPwmRegs->CMPCTL.bit.SHDWBMODE = CC_SHADOW;
        EPwmRegs->CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;
    }

    // Set actions
    if (PWM_Mode == Foc_mode) { // FOCģʽ �������
        EPwmRegs->DBCTL.bit.IN_MODE = DBA_ALL;    // PWM�������� ���û�������
        EPwmRegs->DBCTL.bit.POLSEL = DB_ACTV_LOC; // �ߵ�ƽ��Ч
        EPwmRegs->DBCTL.bit.OUT_MODE = DB_FULL_ENABLE; // ˫����ʱ
        EPwmRegs->DBRED = DEAD_TIME;  // ��������������ʱ��
        EPwmRegs->DBFED = DEAD_TIME;  // �����½�������ʱ��
        //         EPwmRegs->AQCTLA.bit.CAU = AQ_CLEAR;
        //         EPwmRegs->AQCTLA.bit.CAD = AQ_SET;
        //         EPwmRegs->AQCTLB.bit.CBU = AQ_SET;
        //         EPwmRegs->AQCTLB.bit.CBD = AQ_CLEAR;

        EPwmRegs->AQCTLA.bit.CAU = AQ_SET;
        EPwmRegs->AQCTLA.bit.CAD = AQ_CLEAR;
        EPwmRegs->AQCTLB.bit.CBU = AQ_CLEAR;
        EPwmRegs->AQCTLB.bit.CBD = AQ_SET;

    } else if (PWM_Mode == H_PWM_L_PWM_mode) { // H_PWM_L_PWM_modeģʽ���������
        EPwmRegs->AQCTLA.bit.CAU = AQ_SET;
        EPwmRegs->AQCTLA.bit.CAD = AQ_CLEAR;
        EPwmRegs->AQCTLB.bit.CBU = AQ_SET;
        EPwmRegs->AQCTLB.bit.CBD = AQ_CLEAR;
    } else if (PWM_Mode == H_PWM_L_ON_mode) {
        EPwmRegs->AQCTLA.bit.CAU = AQ_SET;   // Set PWM1A on event A, up count
        EPwmRegs->AQCTLA.bit.CAD = AQ_CLEAR; // Clear PWM1A on event A, down count
    }

    EPwm1Regs.ETSEL.bit.SOCAEN = 1;        // ʹ��ePWMa����ADת��
    EPwm1Regs.ETSEL.bit.SOCASEL = ET_CTR_PRD; // ������2250���д���
    EPwm1Regs.ETPS.bit.SOCAPRD = ET_3RD;   // �ڵ�һ�������¼��󼴲�����������

    EALLOW;
    SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1; // Start all the timers synced
    EDIS;
 }


