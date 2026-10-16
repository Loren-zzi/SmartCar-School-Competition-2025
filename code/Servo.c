/*
 * Servo.c
 *
 *  Created on: 2025Äê11ÔÂ14ÈÕ
 *      Author: zhigu
 */


#include "Servo.h"

void Servo_Init(void)
{
    pwm_init(ATOM1_CH1_P33_9,50,SERVO_MID);//SERVO_MID

}


void Servo_Ctrl(uint16 pwm)
{
    uint16 c;

    if   (pwm>SERVO_MAX)
        {
            c=SERVO_MAX;
        }
    else if(pwm<SERVO_MIN)
        {
            c=SERVO_MIN;
        }
    else
        {
            c=pwm;
        }
    pwm_set_duty(ATOM1_CH1_P33_9,c);
}
