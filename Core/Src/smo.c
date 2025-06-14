/*
 * smo.c
 *
 *  Created on: 2025年6月4日
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

    smo->K = 0.05f; // 滑模增益，初始为0
    smo->Ts = Ts; // 采样时间

    smo->Kp_pll = 500; // PLL比例增益，初始为0
    smo->Ki_pll = 50000; // PLL积分增益，初始为0

    smo->Speed_pll = 0.0f; // PLL速度，初始为0
    smo->Theta_pll = 0.0f; // PLL角度，初始为0
    smo->Speed = 0.0f; // 机械角速度，初始为0

    smo->F = 1 - Ts * R / L; // 初始化F
    smo->G = Ts / L; // 初始化G

}

// 滑模切换函数（使用饱和函数来减少抖振）
static float SMO_SwitchFunction(float x, float threshold){
    if (x > threshold)
        return 1.0f;
    else if (x < -threshold)
        return -1.0f;
    else
        return x / threshold;
}

// 滑模观测器更新函数
void SMO_Update(struct SMO *smo, float ialpha, float ibeta, float Ualpha, float Ubeta,float K_fil) {
    smo->ialpha_hat = smo->F * smo->ialpha_hat + smo->G * (Ualpha - smo->ealpha_hat);
    smo->ibeta_hat  = smo->F * smo->ibeta_hat  + smo->G * (Ubeta - smo->ebeta_hat);

    smo->ealpha_hat = smo->K * SMO_SwitchFunction(smo->ialpha_hat - ialpha, 0.1f);
    smo->ebeta_hat  = smo->K * SMO_SwitchFunction(smo->ibeta_hat  - ibeta,  0.1f);

    smo->ealpha_filter = (1 - K_fil) * smo->ealpha_filter + K_fil * smo->ealpha_hat;
    smo->ebeta_filter  = (1 - K_fil) * smo->ebeta_filter  + K_fil * smo->ebeta_hat;

    SMO_PLL_Update(smo); // 更新PLL
}

// PLL更新函数
void SMO_PLL_Update(struct SMO *smo) {
    static float E_error = 0,integral = 0;
    static float speed = 0;

    E_error = -smo->ealpha_filter * cosf(smo->Theta_pll) - smo->ebeta_filter * sinf(smo->Theta_pll);
//    E_error = -smo->ealpha_hat * cosf(smo->Theta_pll) - smo->ebeta_hat * sinf(smo->Theta_pll);

    integral += E_error * smo->Ts; // 积分误差
    smo->Speed_pll = smo->Kp_pll * E_error + smo->Ki_pll * integral; // PLL电角速度更新
    smo->Theta_pll += smo->Speed_pll * smo->Ts; // PLL电角度更新

    // smo->Speed = smo->Speed_pll / 3 / (2 * M_PI) * 60; // 机械角速度更新，电机极对数为3
    speed = smo->Speed_pll * 3.18f;//简化计算
    smo->Speed = speed * 0.001f + smo->Speed * 0.999f; // 滤波器更新机械角速度

    if (smo->Theta_pll > 2 * M_PI) {
        smo->Theta_pll -= 2 * M_PI; // 确保角度在0到2π之间
    } 
    else if (smo->Theta_pll < 0) {
        smo->Theta_pll += 2 * M_PI; // 确保角度在0到2π之间
    }
}

