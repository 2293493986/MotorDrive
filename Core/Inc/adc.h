/*
 * adc.h
 *
 *  Created on: 2025Äê6ÔÂ2ÈÕ
 *      Author: 22934
 */

#ifndef CORE_INC_ADC_H_
#define CORE_INC_ADC_H_

#include "F2806x_Device.h"
#include "F2806x_Examples.h"

struct Curr_data{
    float Ia;
    float Ib;
    float Ic;
};


void Adc_Init_Epwm();
interrupt void Adc_get_data();

extern struct Curr_data Curr_threephase;
extern float V_all;
#endif /* CORE_INC_ADC_H_ */
