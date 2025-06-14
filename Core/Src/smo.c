/*
 * smo.c
 *
 *  Created on: 2025��6��4��
 *      Author: 22934
 */

#include "smo.h"

struct SMO smo;


void SMO_Init(struct SMO *smo, float Ts, float L, float R) {
    smo->ialpha_hat = 0.0f;
    smo->ibeta_hat = 0.0f;
    smo->ealpha_hat = 0.0f;
    smo->ebeta_hat = 0.0f;
    smo->ealpha_filter = 0.0f;
    smo->ebeta_filter = 0.0f;

    smo->K = 0.05f; // ��ģ���棬��ʼΪ0
    smo->Ts = Ts; // ����ʱ��

    smo->Kp_pll = 500; // PLL�������棬��ʼΪ0
    smo->Ki_pll = 50000; // PLL�������棬��ʼΪ0

    smo->Speed_pll = 0.0f; // PLL�ٶȣ���ʼΪ0
    smo->Theta_pll = 0.0f; // PLL�Ƕȣ���ʼΪ0
    smo->Speed = 0.0f; // ��е���ٶȣ���ʼΪ0

    smo->F = 1 - Ts * R / L; // ��ʼ��F
    smo->G = Ts / L; // ��ʼ��G

}

// ��ģ�л�������ʹ�ñ��ͺ��������ٶ���
static float SMO_SwitchFunction(float x, float threshold){
    if (x > threshold)
        return 1.0f;
    else if (x < -threshold)
        return -1.0f;
    else
        return x / threshold;
}

// ��ģ�۲������º���
void SMO_Update(struct SMO *smo, float ialpha, float ibeta, float Ualpha, float Ubeta,float K_fil) {
    smo->ialpha_hat = smo->F * smo->ialpha_hat + smo->G * (Ualpha - smo->ealpha_hat);
    smo->ibeta_hat  = smo->F * smo->ibeta_hat  + smo->G * (Ubeta - smo->ebeta_hat);

    smo->ealpha_hat = smo->K * SMO_SwitchFunction(smo->ialpha_hat - ialpha, 0.1f);
    smo->ebeta_hat  = smo->K * SMO_SwitchFunction(smo->ibeta_hat  - ibeta,  0.1f);

    smo->ealpha_filter = (1 - K_fil) * smo->ealpha_filter + K_fil * smo->ealpha_hat;
    smo->ebeta_filter  = (1 - K_fil) * smo->ebeta_filter  + K_fil * smo->ebeta_hat;

    SMO_PLL_Update(smo); // ����PLL
}

// PLL���º���
void SMO_PLL_Update(struct SMO *smo) {
    static float E_error = 0,integral = 0;
    static float speed = 0;

    E_error = -smo->ealpha_filter * cosf(smo->Theta_pll) - smo->ebeta_filter * sinf(smo->Theta_pll);
//    E_error = -smo->ealpha_hat * cosf(smo->Theta_pll) - smo->ebeta_hat * sinf(smo->Theta_pll);

    integral += E_error * smo->Ts; // �������
    smo->Speed_pll = smo->Kp_pll * E_error + smo->Ki_pll * integral; // PLL����ٶȸ���
    smo->Theta_pll += smo->Speed_pll * smo->Ts; // PLL��Ƕȸ���

    // smo->Speed = smo->Speed_pll / 3 / (2 * M_PI) * 60; // ��е���ٶȸ��£����������Ϊ3
    speed = smo->Speed_pll * 3.18f;//�򻯼���
    smo->Speed = speed * 0.001f + smo->Speed * 0.999f; // �˲������»�е���ٶ�

    if (smo->Theta_pll > 2 * M_PI) {
        smo->Theta_pll -= 2 * M_PI; // ȷ���Ƕ���0��2��֮��
    } 
    else if (smo->Theta_pll < 0) {
        smo->Theta_pll += 2 * M_PI; // ȷ���Ƕ���0��2��֮��
    }
}

