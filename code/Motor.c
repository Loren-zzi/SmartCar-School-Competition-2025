/*
 * Motor.c
 *
 *  Created on: 2025年11月13日
 *      Author: zhigu
 */


#include "Motor.h"


double speed_tar_left=0;
double speed_tar_right=0;



int32 Duty_Left  = 0,
      Duty_Right = 0;  // 占空比 最大 10000

//float target_angle = 50.0;  // 角度目标值

/**------------------
  * @brief  电机初始化
-------------------*/
void Motor_Init(void)
{
    pwm_init(PWM_Left , 17*1000, 0);      //最大值为10000
    pwm_init(PWM_Right, 17*1000, 0);

    gpio_set_dir(Dir_Left,GPO,GPO_PUSH_PULL);
    gpio_set_dir(Dir_Right,GPO,GPO_PUSH_PULL);


}
//===============================================================
void Motor_Pid_Calculate(void)
{
    Duty_Left  = Get_Incremental_PID_Value(&motor_pid[0], speed_tar_left  - speed1);
    Duty_Right = Get_Incremental_PID_Value(&motor_pid[1], speed_tar_right - speed2);
}

//===============================================================

void Motor_Control(void)
{
    Motor_Pid_Calculate();

    if (Duty_Left >= 0)
    {
        gpio_set_level(Dir_Left, 0);
        pwm_set_duty(PWM_Left, Duty_Left);
    }
    else
    {
        gpio_set_level(Dir_Left, 1);
        pwm_set_duty(PWM_Left, -Duty_Left);
    }

    if (Duty_Right >= 0)
    {
        gpio_set_level(Dir_Right, 0);
        pwm_set_duty(PWM_Right, Duty_Right);
    }
    else
    {
        gpio_set_level(Dir_Right, 1);
        pwm_set_duty(PWM_Right, -Duty_Right);
    }
}



void Motor_Set_speed(double Set_speed1,double Set_speed2)
{
    speed_tar_left=Set_speed1;
    speed_tar_right=Set_speed2;
}




