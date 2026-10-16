/*
 * car_control.c
 *
 *  Created on: 2025年11月14日
 *      Author: zhigu
 */


#include "car_control.h"
#include "image.h"




float Sum = 0;
uint16_t Weigth_Sum = 0;
uint8_t midline_fff,midline_ff,midline_f;


void fit_midline(void)//拟和中线，并进行滤波
{
    uint8 i;
    for(i=MT9V03X_H-2; i>=1; i--)
    {
        if(Mid_Line[i]-Mid_Line[i+1]>0)
        {
            if(Mid_Line[i]-Mid_Line[i+1]>8)
            {
                Mid_Line[i]=Mid_Line[i+1]+4;
            }
        }
        else if(Mid_Line[i]-Mid_Line[i+1]<0)
        {
            if(Mid_Line[i+1]-Mid_Line[i]>8)
            {
                Mid_Line[i]=Mid_Line[i+1]-4;
            }
        }
    }
}

void HDPJ_lvbo(void)
{
    uint8 i;
    for(i=MT9V03X_H-3; i>=3; i--)
    {
        End_Mid_Line[i]=(Mid_Line[i+2]
                         +Mid_Line[i+1]
                         +Mid_Line[i]
                         +Mid_Line[i-1]
                         +Mid_Line[i-2])/5;
    }
}


//// 在控制循环中
//void Control_Loop(void)
//{
//    // 1. 图像处理（在CPU1）
//    image_process();  // 生成center_line和End_Mid_Line
//
//    // 2. 转向控制（在CPU0中断）
//    float deviation = Cal_Weigth();  // 使用处理后的中线计算偏差
//
//    // 3. 舵机控制
//    uint16 servo_pwm = SERVO_MID - (int16_t)(deviation * steering_gain);
//    Servo_Ctrl(servo_pwm);
//
//    // 4. 速度控制
//    Speed_control();
//}
//


//
//const uint8 Weigth[120]=
//{
//    0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,0,0,0,0,
//    0,0,0,0,0,0,2,2,2,2,
//    2,4,4,4,4,4,5,5,5,5,
//    5,5,5,6,6,6,6,6,6,8,
//    9,9,9,10,10,10,10,10,10,10,
//    10,9,9,9,9,8,8,7,7,7,
//    6,7,7,7,6,6,6,6,6,5,
//    5,5,5,5,4,4,3,3,3,3,
//    0,0,0,0,0,0,0,0,0,0
//};
//
//float Cal_Weigth(void)
//{
//    Sum = 0;
//    Weigth_Sum = 0;
//    float Mid_Error=75.0;
//
//    for(uint8 i=1; i<100; i++)
//    {
//        Sum += End_Mid_Line[i] * Weigth[i];
//        Weigth_Sum+=Weigth[i];
//    }
//    Mid_Error = Sum /Weigth_Sum;
//
//    midline_fff = midline_ff;
//    midline_ff  = midline_f;
//    midline_f = Mid_Error;
//    Mid_Error = midline_fff * 0.20f + midline_ff * 0.50f + midline_f * 0.30f; //自己更改
//    return Mid_Error - (MT9V03X_W/2)+1;
//}
//
//
//

//============================================================

// 使用原权重数组
const uint8 Weigth1[120]=
{
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,2,2,2,2,
    2,4,4,4,4,4,5,5,5,5,
    5,5,5,6,6,6,6,6,6,8,
    9,9,9,10,10,10,10,10,10,10,
    10,9,9,9,9,8,8,7,7,7,
    6,7,7,7,6,6,6,6,6,5,
    5,5,5,5,4,4,3,3,3,3,
    0,0,0,0,0,0,0,0,0,0
};
const uint8 Weigth2[120]=
{
    0,0,0,0,0,0,0,0,0,0,       // 0-9   行
    0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,2,2,2,2,
    2,4,4,4,4,4,5,5,5,5,
    5,5,5,6,6,6,6,6,6,8,
    9,9,9,10,10,10,10,10,10,10,
    10,9,9,9,9,8,8,7,7,7,
    6,7,7,7,6,6,6,6,6,5,
    5,5,5,5,4,4,3,3,3,3,
    0,0,0,0,0,0,0,0,0,0,

    0,0,0,0,0,0,0,0,0,0,       // 10-19 行
    0,0,0,0,0,0,0,0,0,0,       // 10-19 行
};
float Cal_Weigth1(void)
{
    Sum = 0;
    Weigth_Sum = 0;
    float Mid_Error = 0.0f;

    for(uint8 i=1; i<100; i++)
    {
        Sum += End_Mid_Line[i] * Weigth1[i];
        Weigth_Sum += Weigth1[i];
    }

    if(Weigth_Sum > 0) {
        Mid_Error = (float)Sum / (float)Weigth_Sum;
    }

    // 检查数据有效性
    if(Mid_Error > MT9V03X_W || Mid_Error < 0) {
        return 0.0f;  // 返回安全值
    }

    // 滤波处理
    midline_fff = midline_ff;
    midline_ff  = midline_f;
    midline_f = Mid_Error;
    Mid_Error = midline_fff * 0.20f + midline_ff * 0.50f + midline_f * 0.30f;

    // 归一化到 [-1, 1] 范围
    float normalized_deviation = (Mid_Error - (MT9V03X_W/2.0f)) / (MT9V03X_W/2.0f);

    // 限制在合理范围内
    if(normalized_deviation > 1.0f) normalized_deviation = 1.0f;
    if(normalized_deviation < -1.0f) normalized_deviation = -1.0f;

    return normalized_deviation;

}




float Cal_Weigth2(void)
{
    Sum = 0;
    Weigth_Sum = 0;
    float Mid_Error = 75.0f;

    for(uint8 i=1; i<100; i++)
    {
        Sum += End_Mid_Line[i] * Weigth2[i];
        Weigth_Sum += Weigth2[i];
    }

    if(Weigth_Sum > 0) {
        Mid_Error = (float)Sum / (float)Weigth_Sum;
    }

    // 检查数据有效性
    if(Mid_Error > MT9V03X_W || Mid_Error < 0) {
        return 0.0f;  // 返回安全值
    }

    // 滤波处理
    midline_fff = midline_ff;
    midline_ff  = midline_f;
    midline_f = Mid_Error;
    Mid_Error = midline_fff * 0.20f + midline_ff * 0.50f + midline_f * 0.30f;


   //  归一化到 [-1, 1] 范围
    float normalized_deviation = (Mid_Error - (MT9V03X_W/2.0f)) / (MT9V03X_W/2.0f);

    // 限制在合理范围内
    if(normalized_deviation > 1.0f) normalized_deviation = 1.0f;
    if(normalized_deviation < -1.0f) normalized_deviation = -1.0f;

    return normalized_deviation;



}
//
//float Cal_Weigth1(void)
//{
//    Sum = 0;
//    Weigth_Sum = 0;
//    float Mid_Error = 0.0f;
//
//    for(uint8 i=1; i<100; i++)
//    {
//        Sum += End_Mid_Line[i] * Weigth1[i];
//        Weigth_Sum += Weigth1[i];
//    }
//
//    if(Weigth_Sum > 0) {
//        Mid_Error = (float)Sum / (float)Weigth_Sum;
//    }
//
//    // 检查数据有效性
//    if(Mid_Error > MT9V03X_W || Mid_Error < 0) {
//        return 0.0f;  // 返回安全值
//    }
//
//    // 滤波处理
//    midline_fff = midline_ff;
//    midline_ff  = midline_f;
//    midline_f = Mid_Error;
//    Mid_Error = midline_fff * 0.20f + midline_ff * 0.50f + midline_f * 0.30f;
//
//    // 归一化到 [-1, 1] 范围
////    float normalized_deviation = (Mid_Error - (MT9V03X_W/2.0f)) / (MT9V03X_W/2.0f);
////
////    // 限制在合理范围内
////    if(normalized_deviation > 1.0f) normalized_deviation = 1.0f;
////    if(normalized_deviation < -1.0f) normalized_deviation = -1.0f;
//
//   // return normalized_deviation;
//    return Mid_Error-94;
//}
//
//
//
//
//float Cal_Weigth2(void)
//{
//    Sum = 0;
//    Weigth_Sum = 0;
//    float Mid_Error = 75.0f;
//
//    for(uint8 i=1; i<100; i++)
//    {
//        Sum += End_Mid_Line[i] * Weigth2[i];
//        Weigth_Sum += Weigth2[i];
//    }
//
//    if(Weigth_Sum > 0) {
//        Mid_Error = (float)Sum / (float)Weigth_Sum;
//    }
//
//    // 检查数据有效性
//    if(Mid_Error > MT9V03X_W || Mid_Error < 0) {
//        return 0.0f;  // 返回安全值
//    }
//
//    // 滤波处理
//    midline_fff = midline_ff;
//    midline_ff  = midline_f;
//    midline_f = Mid_Error;
//    Mid_Error = midline_fff * 0.20f + midline_ff * 0.50f + midline_f * 0.30f;
//
//
//    // 归一化到 [-1, 1] 范围
////    float normalized_deviation = (Mid_Error - (MT9V03X_W/2.0f)) / (MT9V03X_W/2.0f);
////
////    // 限制在合理范围内
////    if(normalized_deviation > 1.0f) normalized_deviation = 1.0f;
////    if(normalized_deviation < -1.0f) normalized_deviation = -1.0f;
//
//   // return normalized_deviation;
//    return (Mid_Error-94);
//
//
//}
//





















