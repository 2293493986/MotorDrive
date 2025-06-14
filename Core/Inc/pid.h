/*
 * pid.h
 *
 *  Created on: 2025年6月3日
 *      Author: 22934
 */

#ifndef CORE_INC_PID_H_
#define CORE_INC_PID_H_

// PID控制器结构体定义
struct PID_Controller {
    // 控制参数
    float Kp;        // 比例系数
    float Ki;        // 积分系数
    float Kd;        // 微分系数
    
    // 内部状态
    float prev_error;    // 上一次误差
    float integral;      // 积分项
    
    // 限幅参数
    float integral_limit;    // 积分限幅
    float output_max;        // 输出上限
    float output_min;        // 输出下限
    
    // 采样时间
    float Ts;       // 采样时间(s)
};

// 函数原型声明
void PID_Init(struct PID_Controller *pid, float Kp, float Ki, float Kd, float Ts,
              float integral_limit, float output_min, float output_max);
float PID_Calculate(struct PID_Controller *pid, float setpoint, float measurement);
void PID_Reset(struct PID_Controller *pid);

#endif /* CORE_INC_PID_H_ */
