/*
 * pwm.h
 *
 *  Created on: 2025Äê6ÔÂ2ÈÕ
 *      Author: 22934
 */

#ifndef CORE_INC_PWM_H_
#define CORE_INC_PWM_H_

#include "F2806x_Device.h"
#include "F2806x_Examples.h"

enum PWM_State{
    Foc_mode,
    H_PWM_L_PWM_mode,
    H_PWM_L_ON_mode
};

void Motor_pwm_init(enum PWM_State PWM_Mode);
void Epwm_init(enum PWM_State PWM_Mode, int epwm_num);
void Epwm_gpio_init(enum PWM_State PWM_Mode);

#endif /* CORE_INC_PWM_H_ */
