/*
 * Encoder.c
 *
 *  Created on: 2025年11月13日
 *      Author: zhigu
 */


#include "Encoder.h"

double speed1,speed2;
struct RC_Para Encoder_R_Para = {0, 0, 0.25}; // 低通滤波
struct RC_Para Encoder_L_Para = {0, 0, 0.25};


float RCFilter(float value,RC_Filter_pt Filter) //低通滤波
{
    Filter->temp = value;
    Filter->value = (1 - Filter->RC) * Filter->value + Filter->RC * Filter->temp;
 // temp = RC * value + (1 - RC) * temp;
    return Filter->value;
}

RC_Filter_pt RC_Encoder_L_temp = &Encoder_L_Para;
RC_Filter_pt RC_Encoder_R_temp = &Encoder_R_Para;


int16 Encoder_L =0,Encoder_R = 0;        //编码器直接测量
int16 RC_Encoder_L,RC_Encoder_R;           //低通滤波后

void Encoder_Init(void)
{

    encoder_dir_init(TIM5_ENCODER,TIM5_ENCODER_CH1_P10_3,TIM5_ENCODER_CH2_P10_1);
    encoder_dir_init(TIM6_ENCODER,TIM6_ENCODER_CH1_P20_3,TIM6_ENCODER_CH2_P20_0);
}

void Encoder_Get(void)
{

    Encoder_L = -encoder_get_count(TIM5_ENCODER);
    Encoder_R =  encoder_get_count(TIM6_ENCODER);

    RC_Encoder_L  = (int16_t)RCFilter(Encoder_L,RC_Encoder_L_temp);
    RC_Encoder_R  = (int16_t)RCFilter(Encoder_R,RC_Encoder_R_temp);

    //每一圈CNT加2300；
    //速度=RC_Encoder_L/2300/0.005
    speed1=(float)((RC_Encoder_L/2300.0f/0.005f)*10);//*10是为了数值大一些，
    speed2=(float)((RC_Encoder_R/2300.0f/0.005f)*10);
//  速度 = ( (总脉冲数 / 编码器线数)  * 编码器齿数 / 车模齿数 ) * 轮周长 / 脉冲读取时间
//    rpm_LF = ((RC_Encoder_LF/encoder_pulse)*encoder_gear/motor_gear)*wheel_R/0.005;
//    rpm_LB = ((RC_Encoder_LB/encoder_pulse)*encoder_gear/motor_gear)*wheel_R/0.005;
//    rpm_RF = ((RC_Encoder_RF/encoder_pulse)*encoder_gear/motor_gear)*wheel_R/0.005;
//  rpm_RB = ((RC_Encoder_RB/encoder_pulse)*encoder_gear/motor_gear)*wheel_R/0.005;

    encoder_clear_count(TIM5_ENCODER);          //清零
    encoder_clear_count(TIM6_ENCODER);

}




