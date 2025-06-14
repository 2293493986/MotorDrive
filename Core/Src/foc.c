/*
 * foc.c
 *
 *  Created on: 2025年6月2日
 *      Author: 22934
 */

#include "foc.h"

struct Curr Curr_foc;
struct Vol  Vol_foc;

struct PID_Controller PID_Iq;
struct PID_Controller PID_Id;
struct filter filter_data = {0.0f, 0.0f}; // 滤波器数据结构体

void PID_PreLoad(struct PID_Controller *pid, float *paremeters) {
    PID_Init(pid,
              paremeters[0], // Kp
              paremeters[1], // Ki
              paremeters[2], // Kd
              paremeters[3], // Ts
              paremeters[4], // integral_limit
              paremeters[5], // output_min
              paremeters[6]  // output_max
    );
}

void PID_Load(){
    // filter_data.Id = 0.0f; // 初始化Id滤波器
    // filter_data.Iq = 0.0f; // 初始化Iq滤波器
    PID_PreLoad(&PID_Iq, Iq_PID_paremeters);
    PID_PreLoad(&PID_Id, Id_PID_paremeters);
}

/**
 * Clark变换，将三相电流转换为αβ坐标系
 * @param Ia 三相电流A
 * @param Ib 三相电流B
 * @param Ic 三相电流C
 */
void Clark(struct Curr *curr, float Ia, float Ib) {
    curr->Ialpha = Ia;  // α轴电流
    curr->Ibeta = (Ia + 2 * Ib) * 0.57735f; // β轴电流，1.732/3 = 0.57735
}
/**
 * Park变换，将αβ坐标系电压转换为dq坐标系
 * @param curr 电流数据结构体
 * @param sin_angle 电角度的正弦值
 * @param cos_angle 电角度的余弦值
 */
void Park(struct Curr *curr, float sin_angle, float cos_angle) {
    curr->Id = curr->Ialpha * cos_angle + curr->Ibeta * sin_angle;  // 直轴电流
    curr->Iq = -curr->Ialpha * sin_angle + curr->Ibeta * cos_angle; // 交轴电流
}

/**
 * 逆Park变换，将dq坐标系电压转换为αβ坐标系
 * @param vol 电压数据结构体
 * @param sin_angle 电角度的正弦值
 * @param cos_angle 电角度的余弦值
 */
void Ipark(struct Vol *vol, float sin_angle, float cos_angle){
    vol->Ualpha = vol->Ud * cos_angle - vol->Uq * sin_angle;
    vol->Ubeta  = vol->Ud * sin_angle + vol->Uq * cos_angle;
}

/**
 * 空间矢量脉宽调制（SVPWM）算法
 * @param vol 电压数据结构体
 * @param U_all 电源电压值
 */
void Svpwm(struct Vol *vol, float U_all) {
    const float SQRT3 = 1.732f;            // √3
    const float SQRT3_DIV2 = 0.866f;       // √3/2
    const float SCALE = 2250.0f;

    // 使用简化的扇区判断
    unsigned char A = (vol->Ubeta >= 0);
    unsigned char B = (vol->Ualpha * SQRT3 - vol->Ubeta >= 0);
    unsigned char C = (-vol->Ualpha * SQRT3 - vol->Ubeta >= 0);
    unsigned char N = (C << 2) | (B << 1) | A;  // 位运算替代乘法

    // 提取公共因子优化计算
    float inv_U_all = SQRT3 / U_all;
    float Ubeta_scaled = vol->Ubeta * inv_U_all;
    float Ualpha_scaled = vol->Ualpha * inv_U_all;

    float X = Ubeta_scaled;
    float Y = SQRT3_DIV2 * Ualpha_scaled + 0.5f * Ubeta_scaled;
    float Z = -SQRT3_DIV2 * Ualpha_scaled + 0.5f * Ubeta_scaled;

    // 直接计算最终的 Ta, Tb, Tc，跳过中间变量
    float Ta, Tb, Tc;

    switch(N) {
        case 1: {
            // T1 = Z; T2 = Y
            // 计算: V2 = 0.5*(1-(Z)+(Y)), V1 = 0.5*(1+(Z)+(Y)), V3 = 0.5*(1-(Z)-(Y))
            Ta = 0.5f * (1 - Z + Y);  // V2
            Tb = 0.5f * (1 + Z + Y);  // V1
            Tc = 0.5f * (1 - Z - Y);  // V3
            break;
        }
        case 2: {
            // T1 = Y; T2 = -X
            // 计算: V1 = 0.5*(1+(Y)+(-X)), V3 = 0.5*(1-(Y)-(-X)), V2 = 0.5*(1-(Y)+(-X))
            Ta = 0.5f * (1 + Y - X);  // V1
            Tb = 0.5f * (1 - Y + X);  // V3
            Tc = 0.5f * (1 - Y - X);  // V2
            break;
        }
        case 3: {
            // T1 = -Z; T2 = X
            // 计算: V1 = 0.5*(1+(-Z)+(X)), V2 = 0.5*(1-(-Z)+(X)), V3 = 0.5*(1-(-Z)-(X))
            Ta = 0.5f * (1 - Z + X);  // V1
            Tb = 0.5f * (1 + Z + X);  // V2
            Tc = 0.5f * (1 + Z - X);  // V3
            break;
        }
        case 4: {
            // T1 = -X; T2 = Z
            // 计算: V3 = 0.5*(1-(-X)-(Z)), V2 = 0.5*(1-(-X)+(Z)), V1 = 0.5*(1+(-X)+(Z))
            Ta = 0.5f * (1 + X - Z);  // V3
            Tb = 0.5f * (1 + X + Z);  // V2
            Tc = 0.5f * (1 - X + Z);  // V1
            break;
        }
        case 5: {
            // T1 = X; T2 = -Y
            // 计算: V3 = 0.5*(1-(X)-(-Y)), V1 = 0.5*(1+(X)+(-Y)), V2 = 0.5*(1-(X)+(-Y))
            Ta = 0.5f * (1 - X + Y);  // V3
            Tb = 0.5f * (1 + X - Y);  // V1
            Tc = 0.5f * (1 - X - Y);  // V2
            break;
        }
        case 6: {
            // T1 = -Y; T2 = -Z
            // 计算: V2 = 0.5*(1-(-Y)+(-Z)), V3 = 0.5*(1-(-Y)-(-Z)), V1 = 0.5*(1+(-Y)+(-Z))
            Ta = 0.5f * (1 + Y - Z);  // V2
            Tb = 0.5f * (1 + Y + Z);  // V3
            Tc = 0.5f * (1 - Y - Z);  // V1
            break;
        }
        default: {
            Ta = Tb = Tc = 0.5f;
            break;
        }
    }

    // 一次性设置PWM寄存器
    EPwm1Regs.CMPA.half.CMPA = EPwm1Regs.CMPB = SCALE * Ta;
    EPwm2Regs.CMPA.half.CMPA = EPwm2Regs.CMPB = SCALE * Tb;
    EPwm3Regs.CMPA.half.CMPA = EPwm3Regs.CMPB = SCALE * Tc;
}


/**
 * 角度归一化函数，确保角度在[0, 2π)范围内
 * @param angle 要归一化的角度
 * @return 归一化后的角度
 */
static float normalize_angle(float angle){
    const float PI2 = 2.0f * M_PI;
    float a = fmodf(angle, PI2);
    if (a >= 0) {
        return a;
    } else {
        return a + PI2;
    }
}

/**
 * 开环V/F启动函数
 * @param vol 电压数据结构体指针，用于存储计算得到的电压
 * @param angle 指向角度的指针，用于返回当前电角度
 */
void OpenLoopVF_Startup(struct Vol *vol,float *angle){
    // 保持角度状态的静态变量
    static float angle_open_loop = 0.0f;
    static int first_call = 1;  // 用于初始化检查

    // 初始化角度（首次调用时）
    if (first_call) {
        angle_open_loop = 0.0f;
        first_call = 0;
    }

    // 常量定义
    const float PI2 = 2.0f * M_PI;

    // 电机参数
    const float target_rpm = 300.0f;    // 目标机械转速
    const float pole_pairs = 3.0f;      // 极对数
    const float rated_voltage = 48.0f;  // 额定电压
    const float rated_rpm = 7306.0f;    // 额定机械转速

    // 计算电频率和V/f比例
    float electrical_freq = (target_rpm * pole_pairs) / 60.0f;  // 目标电频率
    float rated_electrical_freq = (rated_rpm * pole_pairs) / 60.0f; // 额定电频率
    float V_per_Hz = rated_voltage / rated_electrical_freq;     // 压频比

    // 计算输出电压和角度增量
    vol->Uq = V_per_Hz * electrical_freq;    // 交轴电压
    vol->Ud = 0.0f;                          // 直轴电压置零
    float delta_theta = PI2 * electrical_freq * 0.0002f; // 假设采样时间为1ms

    // 更新角度并归一化
    angle_open_loop = angle_open_loop + delta_theta;
    angle_open_loop = normalize_angle(angle_open_loop);

    // 输出当前角度
    *angle = angle_open_loop;
}

/**
 * V/F开环控制电机总函数
 * 按顺序调用OpenLoopVF_Startup、Ipark和Svpwm函数
 */
void VF_OpenLoop_Control(){
    // 1. 角度变量
    float angle;
    
    // 2. 调用V/F开环函数，计算所需的电压和获取当前角度
    OpenLoopVF_Startup(&Vol_foc, &angle);
    
    // 3. 仅计算一次sin和cos值
    float sin_angle = sinf(angle);
    float cos_angle = cosf(angle);
    
    // 4. 调用Ipark进行坐标变换，将dq轴电压转换为αβ轴
    Ipark(&Vol_foc, sin_angle, cos_angle);
    
    // 5. 调用SVPWM函数生成PWM信号
    Svpwm(&Vol_foc, 48);
}

/**
 * 计算所需的Iq电流以实现目标速度
 * @param target_speed 目标速度
 * @param startup_time 启动时间
 */
float calculateRequiredIq(float target_speed, float startup_time) {
    // 电机参数（应从配置或测量中获取）
    //电磁转矩 Te = Kt * Iq
    // 其中 Kt = 1.5 * p * Φf，p为极对数，Φf为磁链
    //    const float p = 3.0f;         // 极对数
    const float Kt = 0.1f;       // 转矩常数(Nm/A) 
    const float J = 0.0001f;     // 总转动惯量(kg·m?)
    const float T_load = 0.05f;  // 估计负载转矩(Nm)
    const float safety_margin = 0.2f;  // 20%安全余量
    const float B = 0.01f;       // 摩擦系数(Nm·s/rad)，假设值
    
    // 转换RPM到rad/s
    float omega_target = target_speed * M_PI / 30.0f; //机械转速转为rad/s
    
    // 计算加速转矩
    float T_accel = J * (omega_target / startup_time);
    
    // 计算总需求转矩
    float T_required = T_load + T_accel + B * omega_target; // B为摩擦系数，假设为0.01 Nm·s/rad
    
    // 计算所需Iq（带余量）
    float required_iq = (T_required / Kt) * (1 + safety_margin);
    
    // 限制在最大允许电流范围内
    const float I_limit = 2.0f;  // 最大允许电流(A)
    if (required_iq > I_limit) {
        required_iq = I_limit;
    }
    
    return required_iq;
}

/**
 * 开环IF控制函数
 * @param target_speed 目标速度
 * @param startup_time 启动时间
 */
void IF_OpenLoop_Control(float target_speed,float startup_time,float tar_iq){
    static unsigned char State = 0; // 状态机变量
    static float angle = 0.0f,sine = 0.0f,cosine = 0.0f,omega = 0.0f; // 初始化角度及角速度
    static float State_Ts = 0; // 状态机状态切换时间
    static float err_angle = 0.0f; // 角度误差
    switch (State){
        case 0:{
            angle = 0.0f; // 初始化角度
            sine = sinf(angle); // 计算正弦值
            cosine = cosf(angle); // 计算余弦值
            // Clarke(&Curr_foc, Curr_threephase.Ia, Curr_threephase.Ib); // Clarke变换
            // Park(&Curr_foc, sine, cosine); // Park变换 计算直轴和交轴电流
            // Vol_foc.Ud = PID_Calculate(PID_Id, 0.6f, Curr_foc.Id); // 计算直轴电压
            // Vol_foc.Uq = PID_Calculate(PID_Iq, 0.0f, Curr_foc.Iq); // 计算交轴电压
            Vol_foc.Ud = 1.5; // 直轴电压设为1V
            Vol_foc.Uq = 0; // 交轴电压设为0V
            Ipark(&Vol_foc, sine, cosine); // 逆Park变换
            Svpwm(&Vol_foc, V_all); // 调用SVPWM函数生成PWM信号
            if(State_Ts++ >= 2500){
                State_Ts = 0; // 重置状态切换时间
                SMO_Init(&smo,SMO_paremeters[0],SMO_paremeters[1],SMO_paremeters[2]); // 初始化滑模观测器
                State = 1; // 切换到下一个状态
            }
            break;
        }
        case 1:{
            State_Ts += 0.0004f; // 增加状态切换时间
            if(State_Ts < startup_time){
                omega = target_speed * (State_Ts / startup_time) * M_PI / 10;// 线性增加角速度
            }
            else{
                omega = target_speed * M_PI / 10; // 达到目标速度
//                 State_Ts = startup_time; // 状态机状态切换时间
            }
            angle += omega * 0.0004f; // 更新角度
            angle = normalize_angle(angle); // 归一化角度
            sine = sinf(angle); // 计算正弦值
            cosine = cosf(angle); // 计算余弦值
            Clark(&Curr_foc, Curr_threephase.Ia, Curr_threephase.Ib); // Clarke变换
            Park(&Curr_foc, sine, cosine); // Park变换 计算直轴和交轴电流
            Vol_foc.Ud = PID_Calculate(&PID_Id, 0.0f, Curr_foc.Id); // 计算直轴电压
            Vol_foc.Uq = PID_Calculate(&PID_Iq, tar_iq, Curr_foc.Iq); // 计算交轴电压
            Ipark(&Vol_foc, sine, cosine); // 逆Park变换
            Svpwm(&Vol_foc, V_all); // 调用SVPWM函数生成PWM信号
            SMO_Update(&smo, Curr_foc.Ialpha, Curr_foc.Ibeta, Vol_foc.Ualpha, Vol_foc.Ubeta, 0.02f); // 更新滑模观测器
//            err_angle = angle - smo.Theta_pll; // 计算角度误差
            err_angle = 0.0001f * (angle - smo.Theta_pll) + 0.9999f * err_angle;
            //归一化角度误差到[-π, π]范围
            if(err_angle > M_PI) {
                err_angle -= 2 * M_PI;
            } else if(err_angle < -M_PI) {
                err_angle += 2 * M_PI;
            }
//            if(State_Ts > 4 * startup_time){
//                State_Ts = 0;
//                State = 2;
//            }
            break;
        }
        case 2:{
            
            sine = sinf(smo.Theta_pll + 2.45f);
            cosine = cosf(smo.Theta_pll + 2.45f);
            
            Clark(&Curr_foc, Curr_threephase.Ia, Curr_threephase.Ib);
            Park(&Curr_foc, sine, cosine);
            
            Vol_foc.Ud = PID_Calculate(&PID_Id, 0.0f,   Curr_foc.Id);
            Vol_foc.Uq = PID_Calculate(&PID_Iq, tar_iq, Curr_foc.Iq);
            
            Ipark(&Vol_foc, sine, cosine);
            Svpwm(&Vol_foc, V_all);
            SMO_Update(&smo, Curr_foc.Ialpha, Curr_foc.Ibeta, Vol_foc.Ualpha, Vol_foc.Ubeta, 0.02f);
            break;
        }
        default:{
            break;
        }
    }
    filter_data.Id = Curr_foc.Id * 0.0001f + filter_data.Id * 0.9999f; // 更新Id滤波器数据
    filter_data.Iq = Curr_foc.Iq * 0.0001f + filter_data.Iq * 0.9999f; // 更新Iq滤波器数据
}



