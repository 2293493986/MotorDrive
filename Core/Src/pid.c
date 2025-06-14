/*
 * pid.c
 *
 *  Created on: 2025��6��3��
 *      Author: 22934
 */

#include "pid.h"

/**
 * ��ʼ��PID������
 * @param pid PID�������ṹ��ָ��
 * @param Kp ����ϵ��
 * @param Ki ����ϵ��
 * @param Kd ΢��ϵ��
 * @param Ts ����ʱ��(��)
 * @param integral_limit �������޷�
 * @param output_min �������
 * @param output_max �������
 */
void PID_Init(struct PID_Controller *pid, float Kp, float Ki, float Kd, float Ts,
              float integral_limit, float output_min, float output_max) {
    // ���ÿ��Ʋ���
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->Ts = Ts;
    
    // �����޷�
    pid->integral_limit = integral_limit;
    pid->output_min = output_min;
    pid->output_max = output_max;
    
    // ��ʼ��״̬
    pid->prev_error = 0.0f;
    pid->integral = 0.0f;
}

/**
 * PID���㺯��
 * @param pid PID�������ṹ��ָ��
 * @param setpoint �趨ֵ
 * @param measurement ����ֵ
 * @return ����õ��Ŀ������
 */
float PID_Calculate(struct PID_Controller *pid, float setpoint, float measurement) {
    // �������
    float error = setpoint - measurement;
    
    // ���������
    float p_term = pid->Kp * error;
    
    // ���������
    pid->integral += error * pid->Ts;
    
    // �����޷�
    if (pid->integral > pid->integral_limit) {
        pid->integral = pid->integral_limit;
    } else if (pid->integral < -pid->integral_limit) {
        pid->integral = -pid->integral_limit;
    }
    
    float i_term = pid->Ki * pid->integral;
    
    // ����΢����
    float derivative = (error - pid->prev_error) / pid->Ts;
    float d_term = pid->Kd * derivative;
    
    // ���浱ǰ���´μ���ʹ��
    pid->prev_error = error;
    
    // ���������
    float output = p_term + i_term + d_term;
    
    // ����޷�
    if (output > pid->output_max) {
        output = pid->output_max;
    } else if (output < pid->output_min) {
        output = pid->output_min;
    }
    
    return output;
}

/**
 * ����PID������״̬
 * @param pid PID�������ṹ��ָ��
 */
void PID_Reset(struct PID_Controller *pid) {
    pid->prev_error = 0.0f;
    pid->integral = 0.0f;
}
