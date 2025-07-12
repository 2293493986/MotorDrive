/*
 * foc.c
 *
 *  Created on: 2025��6��2��
 *      Author: 22934
 */

#include "foc.h"

struct Curr Curr_foc;
struct Vol  Vol_foc;

struct PID_Controller PID_Iq;
struct PID_Controller PID_Id;
struct PID_Controller PID_Speed;

struct filter filter_data = {0.0f, 0.0f}; // �˲������ݽṹ��


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
    // filter_data.Id = 0.0f; // ��ʼ��Id�˲���
    // filter_data.Iq = 0.0f; // ��ʼ��Iq�˲���
    PID_PreLoad(&PID_Speed, Speed_PID_parameters);
    PID_PreLoad(&PID_Iq, Iq_PID_paremeters);
    PID_PreLoad(&PID_Id, Id_PID_paremeters);
}

/**
 * Clark�任�����������ת��Ϊ��������ϵ
 * @param Ia �������A
 * @param Ib �������B
 * @param Ic �������C
 */
void Clark(struct Curr *curr, float Ia, float Ib) {
    curr->Ialpha = Ia;  // �������
    curr->Ibeta = (Ia + 2 * Ib) * 0.57735f; // ���������1.732/3 = 0.57735
}
/**
 * Park�任������������ϵ��ѹת��Ϊdq����ϵ
 * @param curr �������ݽṹ��
 * @param sin_angle ��Ƕȵ�����ֵ
 * @param cos_angle ��Ƕȵ�����ֵ
 */
void Park(struct Curr *curr, float sin_angle, float cos_angle) {
    curr->Id = curr->Ialpha * cos_angle + curr->Ibeta * sin_angle;  // ֱ�����
    curr->Iq = -curr->Ialpha * sin_angle + curr->Ibeta * cos_angle; // �������
}

/**
 * ��Park�任����dq����ϵ��ѹת��Ϊ��������ϵ
 * @param vol ��ѹ���ݽṹ��
 * @param sin_angle ��Ƕȵ�����ֵ
 * @param cos_angle ��Ƕȵ�����ֵ
 */
void Ipark(struct Vol *vol, float sin_angle, float cos_angle){
    vol->Ualpha = vol->Ud * cos_angle - vol->Uq * sin_angle;
    vol->Ubeta  = vol->Ud * sin_angle + vol->Uq * cos_angle;
}

/**
 * �ռ�ʸ��������ƣ�SVPWM���㷨
 * @param vol ��ѹ���ݽṹ��
 * @param U_all ��Դ��ѹֵ
 */
void Svpwm(struct Vol *vol, float U_all) {
    const float SQRT3 = 1.732f;            // ��3
    const float SQRT3_DIV2 = 0.866f;       // ��3/2
    const float SCALE = 2250.0f;

    // ʹ�ü򻯵������ж�
    unsigned char A = (vol->Ubeta >= 0);
    unsigned char B = (vol->Ualpha * SQRT3 - vol->Ubeta >= 0);
    unsigned char C = (-vol->Ualpha * SQRT3 - vol->Ubeta >= 0);
    unsigned char N = (C << 2) | (B << 1) | A;  // λ��������˷�

    // ��ȡ���������Ż�����
    float inv_U_all = SQRT3 / U_all;
    float Ubeta_scaled = vol->Ubeta * inv_U_all;
    float Ualpha_scaled = vol->Ualpha * inv_U_all;

    float X = Ubeta_scaled;
    float Y = SQRT3_DIV2 * Ualpha_scaled + 0.5f * Ubeta_scaled;
    float Z = -SQRT3_DIV2 * Ualpha_scaled + 0.5f * Ubeta_scaled;

    // ֱ�Ӽ������յ� Ta, Tb, Tc�������м����
    float Ta, Tb, Tc;

    switch(N) {
        case 1: {
            // T1 = Z; T2 = Y
            // ����: V2 = 0.5*(1-(Z)+(Y)), V1 = 0.5*(1+(Z)+(Y)), V3 = 0.5*(1-(Z)-(Y))
            Ta = 0.5f * (1 - Z + Y);  // V2
            Tb = 0.5f * (1 + Z + Y);  // V1
            Tc = 0.5f * (1 - Z - Y);  // V3
            break;
        }
        case 2: {
            // T1 = Y; T2 = -X
            // ����: V1 = 0.5*(1+(Y)+(-X)), V3 = 0.5*(1-(Y)-(-X)), V2 = 0.5*(1-(Y)+(-X))
            Ta = 0.5f * (1 + Y - X);  // V1
            Tb = 0.5f * (1 - Y + X);  // V3
            Tc = 0.5f * (1 - Y - X);  // V2
            break;
        }
        case 3: {
            // T1 = -Z; T2 = X
            // ����: V1 = 0.5*(1+(-Z)+(X)), V2 = 0.5*(1-(-Z)+(X)), V3 = 0.5*(1-(-Z)-(X))
            Ta = 0.5f * (1 - Z + X);  // V1
            Tb = 0.5f * (1 + Z + X);  // V2
            Tc = 0.5f * (1 + Z - X);  // V3
            break;
        }
        case 4: {
            // T1 = -X; T2 = Z
            // ����: V3 = 0.5*(1-(-X)-(Z)), V2 = 0.5*(1-(-X)+(Z)), V1 = 0.5*(1+(-X)+(Z))
            Ta = 0.5f * (1 + X - Z);  // V3
            Tb = 0.5f * (1 + X + Z);  // V2
            Tc = 0.5f * (1 - X + Z);  // V1
            break;
        }
        case 5: {
            // T1 = X; T2 = -Y
            // ����: V3 = 0.5*(1-(X)-(-Y)), V1 = 0.5*(1+(X)+(-Y)), V2 = 0.5*(1-(X)+(-Y))
            Ta = 0.5f * (1 - X + Y);  // V3
            Tb = 0.5f * (1 + X - Y);  // V1
            Tc = 0.5f * (1 - X - Y);  // V2
            break;
        }
        case 6: {
            // T1 = -Y; T2 = -Z
            // ����: V2 = 0.5*(1-(-Y)+(-Z)), V3 = 0.5*(1-(-Y)-(-Z)), V1 = 0.5*(1+(-Y)+(-Z))
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

    // һ��������PWM�Ĵ���
    EPwm1Regs.CMPA.half.CMPA = EPwm1Regs.CMPB = SCALE * Ta;
    EPwm2Regs.CMPA.half.CMPA = EPwm2Regs.CMPB = SCALE * Tb;
    EPwm3Regs.CMPA.half.CMPA = EPwm3Regs.CMPB = SCALE * Tc;
}


/**
 * �Ƕȹ�һ��������ȷ���Ƕ���[0, 2��)��Χ��
 * @param angle Ҫ��һ���ĽǶ�
 * @return ��һ����ĽǶ�
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
 * ����V/F��������
 * @param vol ��ѹ���ݽṹ��ָ�룬���ڴ洢����õ��ĵ�ѹ
 * @param angle ָ��Ƕȵ�ָ�룬���ڷ��ص�ǰ��Ƕ�
 */
void OpenLoopVF_Startup(struct Vol *vol,float *angle){
    // ���ֽǶ�״̬�ľ�̬����
    static float angle_open_loop = 0.0f;
    static int first_call = 1;  // ���ڳ�ʼ�����

    // ��ʼ���Ƕȣ��״ε���ʱ��
    if (first_call) {
        angle_open_loop = 0.0f;
        first_call = 0;
    }

    // ��������
    const float PI2 = 2.0f * M_PI;

    // �������
    const float target_rpm = 300.0f;    // Ŀ���еת��
    const float pole_pairs = 3.0f;      // ������
    const float rated_voltage = 48.0f;  // ���ѹ
    const float rated_rpm = 7306.0f;    // ���еת��

    // �����Ƶ�ʺ�V/f����
    float electrical_freq = (target_rpm * pole_pairs) / 60.0f;  // Ŀ���Ƶ��
    float rated_electrical_freq = (rated_rpm * pole_pairs) / 60.0f; // ���Ƶ��
    float V_per_Hz = rated_voltage / rated_electrical_freq;     // ѹƵ��

    // ���������ѹ�ͽǶ�����
    vol->Uq = V_per_Hz * electrical_freq;    // �����ѹ
    vol->Ud = 0.0f;                          // ֱ���ѹ����
    float delta_theta = PI2 * electrical_freq * 0.0002f; // �������ʱ��Ϊ1ms

    // ���½ǶȲ���һ��
    angle_open_loop = angle_open_loop + delta_theta;
    angle_open_loop = normalize_angle(angle_open_loop);

    // �����ǰ�Ƕ�
    *angle = angle_open_loop;
}

/**
 * V/F�������Ƶ���ܺ���
 * ��˳�����OpenLoopVF_Startup��Ipark��Svpwm����
 */
void VF_OpenLoop_Control(){
    // 1. �Ƕȱ���
    float angle;
    
    // 2. ����V/F������������������ĵ�ѹ�ͻ�ȡ��ǰ�Ƕ�
    OpenLoopVF_Startup(&Vol_foc, &angle);
    
    // 3. ������һ��sin��cosֵ
    float sin_angle = sinf(angle);
    float cos_angle = cosf(angle);
    
    // 4. ����Ipark��������任����dq���ѹת��Ϊ������
    Ipark(&Vol_foc, sin_angle, cos_angle);
    
    // 5. ����SVPWM��������PWM�ź�
    Svpwm(&Vol_foc, 48);
}

/**
 * ���������Iq������ʵ��Ŀ���ٶ�
 * @param target_speed Ŀ���ٶ�
 * @param startup_time ����ʱ��
 */
float calculateRequiredIq(float target_speed, float startup_time) {
    // ���������Ӧ�����û�����л�ȡ��
    //���ת�� Te = Kt * Iq
    // ���� Kt = 1.5 * p * ��f��pΪ����������fΪ����
    //    const float p = 3.0f;         // ������
    const float Kt = 0.1f;       // ת�س���(Nm/A) 
    const float J = 0.0001f;     // ��ת������(kg��m?)
    const float T_load = 0.05f;  // ���Ƹ���ת��(Nm)
    const float safety_margin = 0.2f;  // 20%��ȫ����
    const float B = 0.01f;       // Ħ��ϵ��(Nm��s/rad)������ֵ
    
    // ת��RPM��rad/s
    float omega_target = target_speed * M_PI / 30.0f; //��еת��תΪrad/s
    
    // �������ת��
    float T_accel = J * (omega_target / startup_time);
    
    // ����������ת��
    float T_required = T_load + T_accel + B * omega_target; // BΪĦ��ϵ��������Ϊ0.01 Nm��s/rad
    
    // ��������Iq����������
    float required_iq = (T_required / Kt) * (1 + safety_margin);
    
    // ������������������Χ��
    const float I_limit = 2.0f;  // ����������(A)
    if (required_iq > I_limit) {
        required_iq = I_limit;
    }
    
    return required_iq;
}


           
float Target_speed = 1000.0f; // rpm
float iq_cmd = 0;
float Err_angle = 0.0f,Err_angle_encoder = 0;
float encoder_offset11 = 0;
/**
 * ����IF���ƺ���
 * @param target_speed Ŀ���ٶ�
 * @param startup_time ����ʱ��
 */
void IF_OpenLoop_Control(float target_speed,float startup_time,float tar_iq,float angle_encoder){
    static unsigned char State = 0; // ״̬������
    static float angle = 0.0f,sine = 0.0f,cosine = 0.0f,omega = 0.0f; // ��ʼ���Ƕȼ����ٶ�
    static float State_Ts = 0; // ״̬��״̬�л�ʱ��
    static float err_angle = 0.0f; // �Ƕ����
    static float encoder_offset = 0;
    switch (State){
        case 0:{
            angle = 0.0f; // ��ʼ���Ƕ�
            sine = sinf(angle); // ��������ֵ
            cosine = cosf(angle); // ��������ֵ
            // Clarke(&Curr_foc, Curr_threephase.Ia, Curr_threephase.Ib); // Clarke�任
            // Park(&Curr_foc, sine, cosine); // Park�任 ����ֱ��ͽ������
            // Vol_foc.Ud = PID_Calculate(PID_Id, 0.6f, Curr_foc.Id); // ����ֱ���ѹ
            // Vol_foc.Uq = PID_Calculate(PID_Iq, 0.0f, Curr_foc.Iq); // ���㽻���ѹ
            Vol_foc.Ud = 1.5; // ֱ���ѹ��Ϊ1V
            Vol_foc.Uq = 0; // �����ѹ��Ϊ0V
            Ipark(&Vol_foc, sine, cosine); // ��Park�任
            Svpwm(&Vol_foc, V_all); // ����SVPWM��������PWM�ź�
            if(State_Ts++ >= 5000){
                State_Ts = 0; // ����״̬�л�ʱ��
                encoder_offset = angle_encoder;
                SMO_Init(&smo,SMO_paremeters[0],SMO_paremeters[1],SMO_paremeters[2]); // ��ʼ����ģ�۲���
                State = 1; // �л�����һ��״̬
            }
            break;
        }
        case 1:{
            State_Ts += 0.0004f; // ����״̬�л�ʱ��
            if(State_Ts < startup_time){
                omega = target_speed * (State_Ts / startup_time) * M_PI / 10;// �������ӽ��ٶ�
            }
            else{
                omega = target_speed * M_PI / 10; // �ﵽĿ���ٶ�
//                 State_Ts = startup_time; // ״̬��״̬�л�ʱ��
            }
            angle += omega * 0.0004f; // ���½Ƕ�
            angle = normalize_angle(angle); // ��һ���Ƕ�
            sine = sinf(angle); // ��������ֵ
            cosine = cosf(angle); // ��������ֵ
            Clark(&Curr_foc, Curr_threephase.Ia, Curr_threephase.Ib); // Clarke�任
            Park(&Curr_foc, sine, cosine); // Park�任 ����ֱ��ͽ������
            Vol_foc.Ud = PID_Calculate(&PID_Id, 0.0f, Curr_foc.Id);// - smo.Speed_pll * 0.0006 * Curr_foc.Iq; // ����ֱ���ѹ
            Vol_foc.Uq = PID_Calculate(&PID_Iq, tar_iq, Curr_foc.Iq);// + smo.Speed_pll * 0.0166f + smo.Speed_pll * 0.0006f * Curr_foc.Id; // ���㽻���ѹ
            Ipark(&Vol_foc, sine, cosine); // ��Park�任
            Svpwm(&Vol_foc, V_all); // ����SVPWM��������PWM�ź�
            SMO_Update(&smo, Curr_foc.Ialpha, Curr_foc.Ibeta, Vol_foc.Ualpha, Vol_foc.Ubeta, 0.02f); // ���»�ģ�۲���
            Err_angle = angle - smo.Theta_pll; // ����Ƕ����
            Err_angle_encoder = angle_encoder - encoder_offset - smo.Theta_pll;
            encoder_offset11 = 6.28f - normalize_angle(angle_encoder - encoder_offset);
//            err_angle = 0.0001f * (angle - smo.Theta_pll) + 0.9999f * err_angle;
            //��һ���Ƕ���[-��, ��]��Χ
//            if(Err_angle > M_PI) {
//                Err_angle -= 2 * M_PI;
//            } else if(Err_angle < -M_PI) {
//                Err_angle += 2 * M_PI;
//            }
//            if(Err_angle_encoder > M_PI) {
//                Err_angle_encoder -= 2 * M_PI;
//            } else if(Err_angle_encoder < -M_PI) {
//                Err_angle_encoder += 2 * M_PI;
//            }
            if(State_Ts > 5 * startup_time){
                State_Ts = 0;
//                PID_Reset(&PID_Id); // ����PID������
//                PID_Reset(&PID_Iq); // ����PID������
//                smo.Theta_pll = angle;
//                sine = sinf(smo.Theta_pll + err_angle);
//                cosine = cosf(smo.Theta_pll + err_angle);
                State = 2;
            }
            break;
        }
        case 2:{
            
//            sine = sinf(smo.Theta_pll + 2.45f);
//            cosine = cosf(smo.Theta_pll + 2.45f);

            // ʹ��PID�����������������
//            iq_cmd = PID_Calculate(&PID_Speed, Target_speed, smo.Speed);
//            angle += omega * 0.0004f; // ���½Ƕ�
//            angle = normalize_angle(angle); // ��һ���Ƕ�
            sine = sinf(smo.Theta_pll + Vol_foc.Uq);
            cosine = cosf(smo.Theta_pll + Vol_foc.Uq);
            
            Err_angle_encoder = angle_encoder - encoder_offset - smo.Theta_pll;
            if(Err_angle_encoder > M_PI) {
                Err_angle_encoder -= 2 * M_PI;
            } else if(Err_angle_encoder < -M_PI) {
                Err_angle_encoder += 2 * M_PI;
            }

            Clark(&Curr_foc, Curr_threephase.Ia, Curr_threephase.Ib);
            Park(&Curr_foc, sine, cosine);
            
//            Vol_foc.Ud = PID_Calculate(&PID_Id, 0.0f,   Curr_foc.Id);
//            Vol_foc.Uq = PID_Calculate(&PID_Iq, tar_iq, Curr_foc.Iq);
            
            Vol_foc.Ud = PID_Calculate(&PID_Id, 0.0f, Curr_foc.Id);// - smo.Speed_pll * 0.0006f * Curr_foc.Iq; // ����ֱ���ѹ
            Vol_foc.Uq = PID_Calculate(&PID_Iq, tar_iq, Curr_foc.Iq);// + smo.Speed_pll * 0.0166 + smo.Speed_pll * 0.0006f * Curr_foc.Id;// ���㽻���ѹ
            Ipark(&Vol_foc, sine, cosine);
            Svpwm(&Vol_foc, V_all);
            SMO_Update(&smo, Curr_foc.Ialpha, Curr_foc.Ibeta, Vol_foc.Ualpha, Vol_foc.Ubeta, 0.02f);

            break;
        }
        default:{
            break;
        }
    }
    filter_data.Id = Curr_foc.Id * 0.0001f + filter_data.Id * 0.9999f; // ����Id�˲�������
    filter_data.Iq = Curr_foc.Iq * 0.0001f + filter_data.Iq * 0.9999f; // ����Iq�˲�������
}


unsigned char state_close = 0;
/**
 * @brief �и�FOC���ƺ�����ʹ���ⲿ��������õĵ�Ƕ�
 * @param electric_angle_rad ��Ƕȣ����ȣ�
 * @param target_id Ŀ��d�����
 * @param target_iq Ŀ��q�����
 */
void Sensor_FOC_Control(float electric_angle_rad, float target_id, float target_iq) {
    float sine, cosine;
    static float encoder_offset = 0,encoder_offset_ts = 0,err_angle = 0;
    
    if(encoder_offset_ts++ <= 5000){
        encoder_offset = Encoder_ElectricalAlign(electric_angle_rad);
        if(encoder_offset_ts == 5000){
            SMO_Init(&smo,SMO_paremeters[0],SMO_paremeters[1],SMO_paremeters[2]); // ��ʼ����ģ�۲���
        }
    }
    else{
    // �����Ƕȵ����Һ�����ֵ
        encoder_offset11 = normalize_angle(electric_angle_rad - encoder_offset);

        sine = sinf(encoder_offset11 + 1.57f);
        cosine = cosf(encoder_offset11 + 1.57f);
        if(encoder_offset_ts++ >= 100000){
            state_close = 1;
        }
        if(state_close == 1){
            sine = sinf(smo.Theta_pll);
            cosine = cosf(smo.Theta_pll);
        }else{
            sine = sinf(electric_angle_rad - encoder_offset);
            cosine = cosf(electric_angle_rad - encoder_offset);
        }

        err_angle = electric_angle_rad - encoder_offset - smo.Theta_pll; // ����Ƕ����

        if(err_angle > M_PI) {
            err_angle -= 2 * M_PI;
        } else if(err_angle < -M_PI) {
            err_angle += 2 * M_PI;
        }
        // Clark�任�����������ת��Ϊ��������ϵ
        Clark(&Curr_foc, Curr_threephase.Ia, Curr_threephase.Ib);

        // Park�任������������ϵ����ת��Ϊdq����ϵ
        // ע�⣺������ת�򲻶ԣ����Գ��Խ�sine��cosine����λ��
        Park(&Curr_foc, sine, cosine);

        // PID����������dq���ѹ
        Vol_foc.Ud = 0;//PID_Calculate(&PID_Id, target_id, Curr_foc.Id);  // d���ѹ����
        Vol_foc.Uq = PID_Calculate(&PID_Iq, target_iq, Curr_foc.Iq); // q���ѹ����

        //    // ��������ת��������Ҫ��һ����С�ĵ�ѹֵ
        //    if(Vol_foc.Uq > 0.0f && Vol_foc.Uq < 2.0f) Vol_foc.Uq = 2.0f;
        //    if(Vol_foc.Ud < 0.0f && Vol_foc.Ud > -2.0f) Vol_foc.Ud = -2.0f;

        // ��ѹ�޷�����ֹ����ĵ�ѹ���
//        if(Vol_foc.Ud > 8.0f) Vol_foc.Ud = 8.0f;
//        if(Vol_foc.Ud < -8.0f) Vol_foc.Ud = -8.0f;
//        if(Vol_foc.Uq > 8.0f) Vol_foc.Uq = 8.0f;
//        if(Vol_foc.Uq < -8.0f) Vol_foc.Uq = -8.0f;

        // ��Park�任����dq����ϵ��ѹת��Ϊ��������ϵ
        Ipark(&Vol_foc, sine, cosine);

        // SVPWM�������������PWM�ź�
        Svpwm(&Vol_foc, V_all);
        SMO_Update(&smo, Curr_foc.Ialpha, Curr_foc.Ibeta, Vol_foc.Ualpha, Vol_foc.Ubeta, 0.02f);



        // �����˲�������
        filter_data.Id = Curr_foc.Id * 0.0001f + filter_data.Id * 0.9999f; // ����Id�˲�������
        filter_data.Iq = Curr_foc.Iq * 0.0001f + filter_data.Iq * 0.9999f; // ����Iq�˲�������
    }
}

/**
 * @brief �򻯵��и�FOC���Ժ����������Ų�����
 * @param electric_angle_rad ��Ƕȣ����ȣ�
 * @param target_iq Ŀ��q�����
 */
void Simple_Sensor_FOC_Test(float electric_angle_rad, float target_iq) {
    float sine, cosine;
    static float encoder_offset = 0,encoder_offset_ts = 0;
    
    if(encoder_offset_ts++ <= 5000){
        encoder_offset = Encoder_ElectricalAlign(electric_angle_rad);
        if(encoder_offset_ts == 5000){
           SMO_Init(&smo,SMO_paremeters[0],SMO_paremeters[1],SMO_paremeters[2]); // ��ʼ����ģ�۲���
       }
    }
    else{
    // �����Ƕȵ����Һ�����ֵ
       encoder_offset11 = normalize_angle(electric_angle_rad - encoder_offset);
       sine = sinf(encoder_offset11);
       cosine = cosf(encoder_offset11);


       // �򻯿��ƣ�ֱ�����õ�ѹ����ʹ�õ�������
       Vol_foc.Ud = -3;              // d���ѹΪ0
       Vol_foc.Uq = 1.0f;  // q���ѹ������Ŀ�����
    
       //    Vol_foc.Ud = 3;              // d���ѹΪ0
       //    Vol_foc.Uq = 6;  // q���ѹ������Ŀ�����

       // ���Ƶ�ѹ��ֵ
       if(Vol_foc.Uq > 6.0f) Vol_foc.Uq = 6.0f;
       if(Vol_foc.Uq < -6.0f) Vol_foc.Uq = -6.0f;
       Clark(&Curr_foc, Curr_threephase.Ia, Curr_threephase.Ib); // Clarke�任
       Park(&Curr_foc, sine, cosine); // Park�任 ����ֱ��ͽ������
       // ��Park�任
       Ipark(&Vol_foc, sine, cosine);

       // SVPWM���
       Svpwm(&Vol_foc, V_all);
       SMO_Update(&smo, Curr_foc.Ialpha, Curr_foc.Ibeta, Vol_foc.Ualpha, Vol_foc.Ubeta, 0.02f);

       filter_data.Id = Curr_foc.Id * 0.0001f + filter_data.Id * 0.9999f; // ����Id�˲�������
       filter_data.Iq = Curr_foc.Iq * 0.0001f + filter_data.Iq * 0.9999f; // ����Iq�˲�������
    }
}



float Encoder_ElectricalAlign(float electric_angle_rad) {
    // 1. ��ת�Ӿ�ֹ��d��
    Vol_foc.Ud = 3.0f; // �ʵ���ѹ����֤ת������
    Vol_foc.Uq = 0.0f;
    float sin_angle = 0.0f;
    float cos_angle = 1.0f;
    Ipark(&Vol_foc, sin_angle, cos_angle);
    Svpwm(&Vol_foc, V_all);
    // 2. ��¼��ʱ����������Ϊoffset
    return electric_angle_rad;
}


