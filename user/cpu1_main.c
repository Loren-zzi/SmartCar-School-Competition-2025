/********************************************************************************************************************/

#include "zf_common_headfile.h"
#pragma section all "cpu1_dsram"
// 将本语句与#pragma section all restore语句之间的全局变量都放在CPU1的RAM中
// **************************** 代码区域 ****************************

// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设
// 本例程是开源库空工程 可用作移植或者测试各类内外设


void core1_main(void)
{
    disable_Watchdog();                     // 关闭看门狗
    interrupt_global_enable(0);             // 打开全局中断
    // 此处编写用户代码 例如外设初始化代码等

      mt9v03x_init();
      ips200_init(IPS200_TYPE_SPI);

    // 此处编写用户代码 例如外设初始化代码等
      cpu_wait_event_ready();                 // 等待所有核心初始化完毕
    while (TRUE)
    {
        // 此处编写需要循环执行的代码
        if(mt9v03x_finish_flag)
           {
                mt9v03x_finish_flag = 0;
                memcpy(base_image[0],mt9v03x_image[0],MT9V03X_IMAGE_SIZE);

                image_process();

             //=======================================
               ips200_show_gray_image(0, 0, bin_image[0], MT9V03X_W, MT9V03X_H, MT9V03X_W, MT9V03X_H, 0);
             //=============================
//                                       draw_rec(L_U_corner_col,L_U_corner_row,RGB565_PURPLE);
//                                       draw_rec(R_U_corner_col,R_U_corner_row,RGB565_PURPLE);
//                                       draw_rec(L_D_corner_col,L_D_corner_row,RGB565_PURPLE);
//                                       draw_rec(R_D_corner_col,R_D_corner_row,RGB565_PURPLE);

                                       ips200_show_string(0,270,"part:");ips200_show_int(70,270,part,1);
                                       ips200_show_string(130,175,"point:"); ips200_show_int(200,175,turn_point_num,2);
                                       ips200_show_string(130,270,"Zerba:"); ips200_show_int(200,270,Image_Flag.Zerba,1);
                                       ips200_show_string(0,230,"turn_pid.Kp:"); ips200_show_int(140,230,turn_pid.Kp,1);
                                       ips200_show_string(0,250,"deviation:"); ips200_show_float(140,250,deviation,5,2);


           }
    }
}



#pragma section all restore



// **************************** 代码区域 ****************************
// system_delay_us_register(10);

//        //printf("%200",servo_pwm);
//        ips200_show_float(0,200,servo_pwm,5,2);        ips200_show_float(70,200,deviation,5,2);    ips200_show_float(130,200,speed_err,5,2);
//        ips200_show_float(0,200,motor_kp,5,2);        ips200_show_float(70,200,motor_ki,5,2);
////        ips200_show_int(70,255,speed_tar_left,4);

//                        ips200_show_int(200,138,L_U_corner_flag,1);
//                        ips200_show_int(220,138,R_U_corner_flag,1);
//
//                        ips200_show_int(220,158,R_D_corner_flag,1);
//
//
//
//                        draw_rec(L_U_corner_col,L_U_corner_row,RGB565_PURPLE);
//                        draw_rec(R_U_corner_col,R_U_corner_row,RGB565_PURPLE);
//                        draw_rec(L_D_corner_col,L_D_corner_row,RGB565_PURPLE);
//                        draw_rec(R_D_corner_col,R_D_corner_row,RGB565_PURPLE);



//            ips200_show_float(0,200,motor_kp,5,2);        ips200_show_float(70,200,motor_ki,5,2);
//                               ips200_show_float(0,300,speed1,2,3);
//                              //ips200_show_float(70,300,speed1,2,3);
//                              ips200_show_float(70,300,speed2,2,3);
//                              ips200_show_float(0,270,deviation,2,3);
//                              ips200_show_float(70,270,servo_pwm,2,3);

