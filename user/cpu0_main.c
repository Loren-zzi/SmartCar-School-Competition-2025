#include "zf_common_headfile.h"
#include "Beep.h"

//----------------------------------------------------
#pragma section all "cpu0_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU0的RAM中

//30 40

#define base_speed  63.0f  //基础巡线速度

float diff_factor=50.0f;  // 差速系数

// **************************** 代码区域 ****************************
int core0_main(void)
{
    clock_init();                   // 获取时钟频率<务必保留>
    debug_init();                   // 初始化默认调试串口
   // 此处编写用户代码 例如外设初始化代码等


    pit_init(CCU60_CH0, 5000);   //5ms中断
    pit_ms_init(CCU60_CH1,10);


    key_init(5);

    Motor_Init();
    Encoder_Init();
    ALL_PID_Init();

   Servo_Init();

   // Beep_Init();
   //gpio_init(P33_1,GPO,0,GPO_PUSH_PULL); //  gpio_set_level(P33_1,1);
   //Set_Beeptime(1000)

   //system_delay_ms(2000);
    // 此处编写用户代码 例如外设初始化代码等
    cpu_wait_event_ready();         // 等待所有核心初始化完毕
    while (TRUE)
    {

        deviation = Cal_Weigth1();
        servo_pwm = (uint16)((float)SERVO_MID) - (float)(deviation * steering_gain );
      //  servo_pwm = (uint16)((float)SERVO_MID) - (float)(deviation * turn_pid.Kp );
        //fuzzy_pid_xunji(&deviation);


       // Servo_Ctrl(servo_pwm);    //   1045  975     905
        speed_err=(deviation)*diff_factor;
        switch(part)
        {

            case 0:
                        {
                            deviation = Cal_Weigth2();
                            steering_gain = 130.0f;  // 需要较大的增益
                            diff_factor=50.0f;
                            Motor_Set_speed(base_speed+speed_err+20,base_speed-speed_err+20);
                            Servo_Ctrl(servo_pwm);    //   1045  975   905
                        }break;
            case 1:
            {
                deviation = Cal_Weigth1();
                steering_gain = 160.0f;  // 需要较大的增益
                diff_factor=80.0f;
                Motor_Set_speed(base_speed+speed_err+6,base_speed-speed_err+6);
                Servo_Ctrl(servo_pwm);   //   1045  975    905
            }break;

            case 2:
            {
                deviation = Cal_Weigth1();
                steering_gain = 160.0f;  // 需要较大的增益
                diff_factor=80.0f;
                Motor_Set_speed(base_speed+speed_err+10,base_speed-speed_err+10);
                if(servo_pwm<980)
                {
                    servo_pwm=980;
                }
                Servo_Ctrl(servo_pwm);    //   1045  975   905
            }break;

            case 3:
            {
                deviation = Cal_Weigth2();
                steering_gain = 110.0f;  // 需要较大的增益
                diff_factor=30;
                Motor_Set_speed(base_speed+speed_err+20,base_speed-speed_err+20);
//                if(servo_pwm<980)
//                                {
//                             servo_pwm=980;
//                                }
                                Servo_Ctrl(servo_pwm);    //   1045  975   905
              //  Servo_Ctrl(servo_pwm);    //   1045  975    905
            }break;

            case 4:
                        {
                            deviation = Cal_Weigth2();
                            if(deviation<=0.3&&deviation>=-0.3)
                            {
                                steering_gain = 120.0f;  // 需要较大的增益
                                diff_factor=20;
                                Motor_Set_speed(base_speed+speed_err+35,base_speed-speed_err+35);

                            }
                            else
                            {
                               steering_gain = 170.0f;  // 需要较大的增益
                               diff_factor=68;
                               Motor_Set_speed(base_speed+speed_err+25,base_speed-speed_err+25);

                            }


                           // diff_factor=40;

                            Servo_Ctrl(servo_pwm);    //   1045  975     905
                        }break;
            case 5:
            {
                steering_gain = 170.0f;  // 需要较大的增益
                diff_factor=70;
                Motor_Set_speed(base_speed+speed_err+10,base_speed-speed_err+10);
                Servo_Ctrl(servo_pwm);
            }break;
            case 6:
            {
                Motor_Set_speed(-3,-3);
                Servo_Ctrl(975);

            }break;
           // default;
        }


        enable_L_D_corner=1;
        enable_R_D_corner=1;
        enable_L_U_corner=1;
        enable_R_U_corner=1;


    }
}

#pragma section all restore
// **************************** 代码区域 ****************************

//==========================================================================传逐飞助手示波器
//uint32 seekfree_assistant_transfer (const uint8 *buff, uint32 length)
//{
//    uint32 len = 0;
//
//    len = debug_send_buffer(buff, length);
//
//    return len;
//}
//
//seekfree_assistant_oscilloscope_struct oscilloscope_data;
//       oscilloscope_data.data[0] = 0;
//       oscilloscope_data.data[1] = 0;
//       oscilloscope_data.data[2] = 0;
//       oscilloscope_data.channel_num = 3;
// speed_tar_right=30;
//oscilloscope_data.data[0]=speed_tar_right;
//       oscilloscope_data.data[1]=speed1;
//       oscilloscope_data.data[2]=speed2;
//   seekfree_assistant_oscilloscope_send(&oscilloscope_data);
   //system_delay_ms(7);
//
//             if(key_get_state(KEY_1)==KEY_SHORT_PRESS)
//              {
//                  speed_tar_right+=10;
//                  key_clear_all_state();
//              }
//              if(key_get_state(KEY_2)==KEY_SHORT_PRESS)
//                      {
//                  speed_tar_right-=10;
//                          key_clear_all_state();
//                      }
//              if(key_get_state(KEY_3)==KEY_SHORT_PRESS)
//                             {
//                  motor_ki+=0.01;
//                                 key_clear_all_state();
//                             }
//              Motor_Set_speed(speed_tar_right,speed_tar_right);
//


//=======================================================Key_Select
////void Key_Select()
//{
//    switch (Key_Long)
//    {
//    case 1:
//    {
//        Key_Long = 0;
//        num = 5;
//    }
//    break;
//    case 2:
//    {
//        Key_Long = 0;
//    }
//    break;
//    }
//
//    switch (Key_Short)
//    {
//    case 1:
//    {
//        Key_Short = 0;
//        num = 4;
//    }
//    break;
//    case 2:
//    {
//        Key_Short = 0;
//        num = 3;
//    }
//    break;
//    case 3:
//    {
//        Key_Short = 0;
//        num = 2;
//    }
//    break;
//    case 4:
//    {
//        Key_Short = 0;
//        num = 1;
//    }
//    break;
//    }
//}

//======================================PID

//seekfree_assistant_oscilloscope_struct oscilloscope_data;
//       oscilloscope_data.data[0] = 0;
//       oscilloscope_data.data[1] = 0;
//       oscilloscope_data.data[2] = 0;
//       oscilloscope_data.channel_num = 3;
//
//       speed_tar_left=30;
//       oscilloscope_data.data[0]=speed_tar_left;
//       oscilloscope_data.data[1]=speed1;
//       oscilloscope_data.data[2]=speed2;
//
//          seekfree_assistant_oscilloscope_send(&oscilloscope_data);
//
//             if(key_get_state(KEY_1)==KEY_SHORT_PRESS)
//              {
//                  speed_tar_left+=5;
//                  key_clear_all_state();
//              }
//              if(key_get_state(KEY_2)==KEY_SHORT_PRESS)
//                      {
//                  motor_kp+=1;
//                          key_clear_all_state();
//                      }
//              if(key_get_state(KEY_3)==KEY_SHORT_PRESS)
//                             {
//                  motor_ki+=0.01;
//                                 key_clear_all_state();
//                             }
//              Motor_Set_speed(speed_tar_left,0);
//

