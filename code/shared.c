/*
 * shared.c
 *
 *  Created on: 2025年11月15日
 *      Author: zhigu
 */


#include "shared.h"



uint16 servo_pwm=0;
float deviation=94;
float steering_gain=0.25;



float speed_err=0;



//================================image



uint8_t enable_L_D_corner=0;            //左下角
uint8_t enable_R_D_corner=0;
uint8_t enable_L_U_corner=0;
uint8_t enable_R_U_corner=0;

//====================================
Image_Status_Flag_t Image_Flag = {0}; // 初始化所有成员为0



uint8_t part=0;

uint8_t cut_flag=0;
uint8_t start=0;
uint8_t out=0;

uint8_t T=0;
