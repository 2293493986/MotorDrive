/*
 * smo.h
 *
 *  Created on: 2025年6月4日
 *      Author: 22934
 */

#ifndef CORE_INC_SMO_H_
#define CORE_INC_SMO_H_

#include <math.h>

struct SMO{
    float ialpha_hat;
    float ibeta_hat;

    float ealpha_hat;
    float ebeta_hat;

    float ealpha_filter;
    float ebeta_filter;

    float K;  // 滑模增益

    float Kp_pll; // PLL比例增益
    float Ki_pll; // PLL积分增益

    float F; //
    float G; //

    float Ts; // 采样时间

    float Speed; // 机械角速度
    float Speed_pll; // PLL原始电角速度
    float Theta_pll; // PLL角度


};

void SMO_Init(struct SMO *smo, float Ts, float L, float R);
void SMO_PLL_Update(struct SMO *smo);
void SMO_Update(struct SMO *smo, float ialpha, float ibeta, float Ualpha, float Ubeta,float K_fil);

extern struct SMO smo;

#endif /* CORE_INC_SMO_H_ */
