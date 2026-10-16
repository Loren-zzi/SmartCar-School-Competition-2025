/*
 * PID.h
 *
 *  Created on: 2025年11月13日
 *      Author: zhigu
 */

#ifndef CODE_PID_H_
#define CODE_PID_H_


#include "zf_common_headfile.h"




typedef struct {
    float Kp;  // 比例系数
    float Ki;  // 积分系数
    float Kd;  // 微分系数

    float Error;  // 本次误差
    float Error_Last;  // 上次误差
    float Error_Pre;  // 上上次误差

    float Out_Put_Min;  // 限幅
    float Out_Put_Max;

    float Output;
} Incremental_PID_TypeDef;

extern float motor_kp ,motor_ki, motor_kd ;


extern Incremental_PID_TypeDef motor_pid[2];
void Incremental_PID_Init(Incremental_PID_TypeDef *PID, float Kp, float Ki, float Kd, float Out_Put_Max, float Out_Put_Min);
float Get_Incremental_PID_Value(Incremental_PID_TypeDef *PID, float Error);
void ALL_PID_Init(void);

#endif /* CODE_PID_H_ */
