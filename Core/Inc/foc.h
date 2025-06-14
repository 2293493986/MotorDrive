/*
 * foc.h
 *
 *  Created on: 2025Äê6ÔÂ2ÈÕ
 *      Author: 22934
 */

#ifndef CORE_INC_FOC_H_
#define CORE_INC_FOC_H_

#include "F2806x_Device.h"
#include "F2806x_Examples.h"

#include "math.h"
#include "adc.h"
#include "pid.h"
#include "paremeter.h"
#include "stdio.h"
#include "smo.h"

struct Curr{
    float Ialpha;
    float Ibeta;
    float Id;
    float Iq;
};

struct Vol{
    float Ualpha;
    float Ubeta;
    float Ud;
    float Uq;
};

struct filter{
    float Id;
    float Iq;
};


void VF_OpenLoop_Control();
void IF_OpenLoop_Control(float target_speed,float startup_time,float tar_iq);
void PID_Load();

extern struct Curr Curr_foc;
extern struct Vol  Vol_foc;

#endif /* CORE_INC_FOC_H_ */


//void Svpwm(struct Vol *vol,float U_all){
//    const float SCALE = 2250.0f;
//    unsigned char A , B , C , N;
//  float Uref1 , Uref2 , Uref3;
//  float X , Y , Z;
//  float T1 , T2;
//  float Ta , Tb , Tc;
//  Uref1 =  vol->Ubeta;
//  Uref2 =  vol->Ualpha * 1.732f - vol->Ubeta;
//  Uref3 = -vol->Ualpha * 1.732f - vol->Ubeta;
//    if(Uref1 >= 0){
//        A = 1;
//    } else {
//        A = 0;
//    }
//    if(Uref2 >= 0){
//        B = 1;
//    } else {
//        B = 0;
//    }
//    if(Uref3 >= 0){
//        C = 1;
//    } else {
//        C = 0;
//    }
//  N = 4*C + 2*B + A;
//  X = 1.732f / U_all * vol->Ubeta;
//  Y = (0.866f * vol->Ualpha + 0.5f * vol->Ubeta) * (1.732f / U_all);
//  Z = (-0.866f*vol->Ualpha + 0.5f * vol->Ubeta) * (1.732f / U_all);
//  switch(N)
//  {
//      case 1:T1 = Z;T2 = Y;break;
//      case 2:T1 = Y;T2 = -X;break;
//      case 3:T1 = -Z;T2 = X;break;
//      case 4:T1 = -X;T2 = Z;break;
//      case 5:T1 = X;T2 = -Y;break;
//      case 6:T1 = -Y;T2 = -Z;break;
//      default:break;
//  }
//  float xx1 = (1 - T1 - T2) / 4.0f;
//  float xx2 = (1 + T1 - T2) / 4.0f;
//  float xx3 = (1 + T1 + T2) / 4.0f;
//  switch(N){
//        case 1:Ta = 1 - 2 * xx2; Tb = 1 - 2 * xx1; Tc = 1 - 2 * xx3; break;
//        case 2:Ta = 1 - 2 * xx1; Tb = 1 - 2 * xx3; Tc = 1 - 2 * xx2; break;
//        case 3:Ta = 1 - 2 * xx1; Tb = 1 - 2 * xx2; Tc = 1 - 2 * xx3; break;
//        case 4:Ta = 1 - 2 * xx3; Tb = 1 - 2 * xx2; Tc = 1 - 2 * xx1; break;
//        case 5:Ta = 1 - 2 * xx3; Tb = 1 - 2 * xx1; Tc = 1 - 2 * xx2; break;
//        case 6:Ta = 1 - 2 * xx2; Tb = 1 - 2 * xx3; Tc = 1 - 2 * xx1; break;
//        default:break;
//  }
//
//
//    float taVal = SCALE * Ta;
//    float tbVal = SCALE * Tb;
//    float tcVal = SCALE * Tc;
//
//    EPwm1Regs.CMPA.half.CMPA = taVal;
//    EPwm1Regs.CMPB           = taVal;
//
//    EPwm2Regs.CMPA.half.CMPA = tbVal;
//    EPwm2Regs.CMPB           = tbVal;
//
//    EPwm3Regs.CMPA.half.CMPA = tcVal;
//    EPwm3Regs.CMPB           = tcVal;
//}
