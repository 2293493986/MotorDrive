/*
 * paremeter.c
 *
 *  Created on: 2025年6月3日
 *      Author: 22934
 */

#include "paremeter.h"
//                           Kp, Ki, Kd,   Ts,       integral_limit, output_min  , output_max
//float Iq_PID_paremeters[7] = {0.6f, 1600,  0,  0.0004f,      500000.0f,        -27.0f,        27.0f}; // Iq PID参数
//float Id_PID_paremeters[7] = {0.6f, 1600,  0,  0.0004f,      500000.0f,        -27.0f,        27.0f}; // Id PID参数

float Iq_PID_paremeters[7] = {1.0f, 50,  0,  0.0004f,      50000000.0f,        -27.0f,        27.0f}; // Iq PID参数
float Id_PID_paremeters[7] = {1.0f, 50,  0,  0.0004f,      50000000.0f,        -27.0f,        27.0f}; // Id PID参数
//                          Ts        L       R
float SMO_paremeters[3] = {0.0004f, 0.0006f, 1.6f}; // SMO参数
