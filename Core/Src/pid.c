/*
 * pid.c
 *
 *  Created on: 2025年6月3日
 *      Author: 22934
 */

#include "pid.h"

/**
 * 初始化PID控制器
 * @param pid PID控制器结构体指针
 * @param Kp 比例系数
 * @param Ki 积分系数
 * @param Kd 微分系数
 * @param Ts 采样时间(秒)
 * @param integral_limit 积分项限幅
 * @param output_min 输出下限
 * @param output_max 输出上限
 */
void PID_Init(struct PID_Controller *pid, float Kp, float Ki, float Kd, float Ts,
              float integral_limit, float output_min, float output_max) {
    // 设置控制参数
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->Ts = Ts;
    
    // 设置限幅
    pid->integral_limit = integral_limit;
    pid->output_min = output_min;
    pid->output_max = output_max;
    
    // 初始化状态
    pid->prev_error = 0.0f;
    pid->integral = 0.0f;
}

/**
 * PID计算函数
 * @param pid PID控制器结构体指针
 * @param setpoint 设定值
 * @param measurement 测量值
 * @return 计算得到的控制输出
 */
float PID_Calculate(struct PID_Controller *pid, float setpoint, float measurement) {
    // 计算误差
    float error = setpoint - measurement;
    
    // 计算比例项
    float p_term = pid->Kp * error;
    
    // 计算积分项
    pid->integral += error * pid->Ts;
    
    // 积分限幅
    if (pid->integral > pid->integral_limit) {
        pid->integral = pid->integral_limit;
    } else if (pid->integral < -pid->integral_limit) {
        pid->integral = -pid->integral_limit;
    }
    
    float i_term = pid->Ki * pid->integral;
    
    // 计算微分项
    float derivative = (error - pid->prev_error) / pid->Ts;
    float d_term = pid->Kd * derivative;
    
    // 保存当前误差供下次计算使用
    pid->prev_error = error;
    
    // 计算总输出
    float output = p_term + i_term + d_term;
    
    // 输出限幅
    if (output > pid->output_max) {
        output = pid->output_max;
    } else if (output < pid->output_min) {
        output = pid->output_min;
    }
    
    return output;
}

/**
 * 重置PID控制器状态
 * @param pid PID控制器结构体指针
 */
void PID_Reset(struct PID_Controller *pid) {
    pid->prev_error = 0.0f;
    pid->integral = 0.0f;
}
