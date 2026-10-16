/*
 * Motor.h
 *
 *  Created on: 2025年11月13日
 *      Author: zhigu
 */

#ifndef CODE_MOTOR_H_
#define CODE_MOTOR_H_

#include "zf_common_headfile.h"


// 定义方向----------
//#define Dir_Left  D13
//#define Dir_Right D0
//
// PWM引脚-----------------------------
#define PWM_Left  ATOM0_CH5_P02_5
#define PWM_Right ATOM0_CH7_P02_7

// 定义方向----------
#define Dir_Left  P02_4
#define Dir_Right P02_6

//#define PWM_Back  PWM2_MODULE3_CHA_D2



extern double speed_tar_left;
extern double speed_tar_right;


void Motor_Init(void);
void Motor_Control(void);
void Motor_Set_speed(double speed1,double speed2);





#endif /* CODE_MOTOR_H_ */
