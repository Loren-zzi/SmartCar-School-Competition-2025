/*
 * fuzzy_pid.h
 *
 *  Created on: 2025年12月13日
 *      Author: zhigu
 */

#ifndef CODE_FUZZY_PID_H_
#define CODE_FUZZY_PID_H_

#include "zf_common_headfile.h"

typedef struct
{
     float Kp;
     float Kd;
} turn_pid_t;
// 声明全局变量
extern turn_pid_t turn_pid;


extern float Fuzzy_P(float E,float EC);
extern float Fuzzy_D(float E,float EC);
extern void fuzzy_pid_xunji(float *Line_Error);





#endif /* CODE_FUZZY_PID_H_ */
