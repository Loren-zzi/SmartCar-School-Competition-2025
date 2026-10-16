/*
 * Servo.h
 *
 *  Created on: 2025Äê11ÔÂ14ÈÕ
 *      Author: zhigu
 */

#ifndef CODE_SERVO_H_
#define CODE_SERVO_H_

#include "zf_common_headfile.h"




//   1050  1000      930
#define  SERVO_MID 975
#define SERVO_MAX 1045
#define SERVO_MIN 905

void Servo_Init(void);
void Servo_Ctrl(uint16 pwm);




#endif /* CODE_SERVO_H_ */
