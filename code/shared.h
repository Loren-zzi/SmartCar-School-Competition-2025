/*
 * shared.h
 *
 *  Created on: 2025年11月15日
 *      Author: zhigu
 */

#ifndef CODE_SHARED_H_
#define CODE_SHARED_H_






#include "zf_common_headfile.h"




extern  uint16 servo_pwm;
extern float deviation;
extern float steering_gain;
extern float speed_err;



//=======================================image


extern uint8_t enable_L_D_corner;
extern uint8_t enable_R_D_corner;
extern uint8_t enable_L_U_corner;
extern uint8_t enable_R_U_corner;


//==========================================十字
// 定义一个图像状态标志的结构体
typedef struct {
    // ... 其他可能的图像标志位 ...

    // 十字路口补线状态标志
    // 0: 未进行十字补线
    // 1: 检测到标准或斜入十字路口，并已进行补线
    // 2: 检测到出十字路口（远端十字），并已进行补线
    uint8_t Cross_Fill;
    uint8_t Zerba;
    // 你还可以添加其他标志，例如：
    // uint8_t In_Roundabout;      // 是否在环岛内
    // uint8_t Obstacle_Detected;  // 是否检测到障碍物

} Image_Status_Flag_t;

// 声明一个全局的实例
extern Image_Status_Flag_t Image_Flag;


extern uint8_t part;

extern uint8_t cut_flag;
extern uint8_t out;

extern uint8_t T;




#endif /* CODE_SHARED_H_ */
