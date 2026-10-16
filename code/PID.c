/*
 * PID.c
 *
 *  Created on: 2025年11月13日
 *      Author: zhigu
 */


#include "PID.h"

Incremental_PID_TypeDef motor_pid[2];
float motor_kp = 50,motor_ki = 0.01, motor_kd = 0.01;
//float motor_kp = 50,motor_ki = 0.0, motor_kd = 0.0;
/**
-- @brief     增量式PID初始化
-- @param     Incremental_PID_TypeDef *PID  PID结构体
-- @param     float Kp 设定的Kp
-- @param     float Ki 设定的Ki
-- @param     float Kd 设定的Kd
-- @param     float outPutMin 限幅
-- @param     float outPutMax 限幅
**/
void Incremental_PID_Init(Incremental_PID_TypeDef *PID, float Kp, float Ki, float Kd, float Out_Put_Max, float Out_Put_Min)
{
    PID -> Kp = Kp;
    PID -> Ki = Ki;
    PID -> Kd = Kd;
    PID -> Error = 0;
    PID -> Error_Last = 0;
    PID -> Error_Pre = 0;
    PID -> Out_Put_Min = Out_Put_Min;
    PID -> Out_Put_Max = Out_Put_Max;
    PID -> Output = 0;  // 添加这行，初始化Output为0
}

//=============================================================================
/**
-- @brief     增量式PID获取值
-- @param     Incremental_PID_TypeDef *PID PID结构体
-- @param     float Target_Value 目标值
-- @param     float Error 误差
-- @return    float Output 输出
**/
float Get_Incremental_PID_Value(Incremental_PID_TypeDef *PID, float Error)
{
    float P = 0.0f, I = 0.0f, D = 0.0f;
    float Increment = 0;  // 增量

    PID -> Error = Error;  // 获取误差

    P = PID -> Error - PID -> Error_Last;  // 当前误差-上一次误差
    I = PID -> Error;
    D = PID -> Error - 2*PID->Error_Last + PID->Error_Pre;
    // 简单的积分项限幅 ---
     // 如果当前输出已经达到最大值，并且误差仍然是正的（意味着还想继续增加），
     // 那么就让积分项不起作用（或只起反向作用），防止积分饱和。
     if ((PID->Output >= PID->Out_Put_Max) && (I > 0))
     {
         I = 0;
     }
     // 如果当前输出已经达到最小值，并且误差仍然是负的（意味着还想继续减小），
     // 那么也让积分项不起作用。
     else if ((PID->Output <= PID->Out_Put_Min) && (I < 0))
     {
         I = 0;
     }

    Increment = PID->Kp*P + PID->Ki*I + PID->Kd*D;

    PID -> Error_Pre = PID -> Error_Last;
    PID -> Error_Last = PID -> Error;

    PID -> Output = PID->Output + Increment;

    // 限幅
    if(PID -> Output > PID -> Out_Put_Max)
    {
        PID -> Output = PID -> Out_Put_Max;
    }
    else if(PID -> Output < PID -> Out_Put_Min)
    {
        PID -> Output = PID -> Out_Put_Min;
    }

    return PID -> Output;
}

//==================================================================================
void ALL_PID_Init(void)
{
    // 初始化俩个电机的增量式PID控制器，用于控制电机速度
       Incremental_PID_Init(&motor_pid[0], motor_kp, motor_ki, motor_kd, 7000, -7000);   //left
       Incremental_PID_Init(&motor_pid[1], motor_kp, motor_ki, motor_kd, 7000, -7000);   //right

}








