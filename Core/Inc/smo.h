/*
 * smo.h
 *
 *  Created on: 2025��6��4��
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

    float K;  // ��ģ����

    float Kp_pll; // PLL��������
    float Ki_pll; // PLL��������

    float F; //
    float G; //

    float Ts; // ����ʱ��

    float Speed; // ��е���ٶ�
    float Speed_pll; // PLLԭʼ����ٶ�
    float Theta_pll; // PLL�Ƕ�


};

void SMO_Init(struct SMO *smo, float Ts, float L, float R);
void SMO_PLL_Update(struct SMO *smo);
void SMO_Update(struct SMO *smo, float ialpha, float ibeta, float Ualpha, float Ubeta,float K_fil);

extern struct SMO smo;

#endif /* CORE_INC_SMO_H_ */
