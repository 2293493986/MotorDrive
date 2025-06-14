/*
 * pid.h
 *
 *  Created on: 2025��6��3��
 *      Author: 22934
 */

#ifndef CORE_INC_PID_H_
#define CORE_INC_PID_H_

// PID�������ṹ�嶨��
struct PID_Controller {
    // ���Ʋ���
    float Kp;        // ����ϵ��
    float Ki;        // ����ϵ��
    float Kd;        // ΢��ϵ��
    
    // �ڲ�״̬
    float prev_error;    // ��һ�����
    float integral;      // ������
    
    // �޷�����
    float integral_limit;    // �����޷�
    float output_max;        // �������
    float output_min;        // �������
    
    // ����ʱ��
    float Ts;       // ����ʱ��(s)
};

// ����ԭ������
void PID_Init(struct PID_Controller *pid, float Kp, float Ki, float Kd, float Ts,
              float integral_limit, float output_min, float output_max);
float PID_Calculate(struct PID_Controller *pid, float setpoint, float measurement);
void PID_Reset(struct PID_Controller *pid);

#endif /* CORE_INC_PID_H_ */
