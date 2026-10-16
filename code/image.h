/*
 * image.h
 *
 *  Created on: 2025年10月1日
 *      Author: zhigu
 */

#ifndef CODE_IMAGE_H_
#define CODE_IMAGE_H_


#include "zf_common_headfile.h"

//=================================
#define image_h 120//图像高度
#define image_w 188//图像宽度

#define white_pixel 255
#define black_pixel 0

#define border_max  image_w-2 //边界最大值
#define border_min  1   //边界最小值

//========================================================
extern uint8 Mid_Line[MT9V03X_H];      // 中线
extern uint8 End_Mid_Line[MT9V03X_H];  // 滤波后中线

extern uint8 Threshold;                             //大津法阈值
extern uint8 base_image[MT9V03X_H][MT9V03X_W];      // 复制图片 (120×180)
extern uint8 bin_image[MT9V03X_H][MT9V03X_W];      // 二值化图像 (120×180)


extern uint8 l_border[image_h];//左线数组
extern uint8 r_border[image_h];//右线数组



#define USE_num image_h*3   //定义找点的数组成员个数按理说300个点能放下，但是有些特殊情况确实难顶，多定义了一点
uint8 otsuThreshold(uint8 *image, uint16 col, uint16 row);
void turn_to_bin(void);
void image_filter(uint8(*bin_image)[MT9V03X_W]);  //形态学滤波，简单来说就是膨胀和腐蚀的思想
void image_draw_rectan(uint8(*image)[MT9V03X_W]);  //画框
uint8 get_start_point(uint8 start_row);
void search_l_r(uint16 break_flag, uint8(*image)[image_w], uint16 *l_stastic, uint16 *r_stastic, uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y, uint8*hightest);


extern uint8_t L_U_corner_flag;    // 左上拐点标志
extern uint8_t L_U_corner_row;     // 左上拐点行坐标
extern uint8_t L_U_corner_col;     // 左上拐点列坐标
extern float L_U_corner_angle;     // 左上拐点角度

extern uint8_t L_D_corner_flag;    // 左下拐点标志
extern uint8_t L_D_corner_row;     // 左下拐点行坐标
extern uint8_t L_D_corner_col;     // 左下拐点列坐标
extern float L_D_corner_angle;     // 左下拐点角度

extern uint8_t R_U_corner_flag;    // 右上拐点标志
extern uint8_t R_U_corner_row;     // 右上拐点行坐标
extern uint8_t R_U_corner_col;     // 右上拐点列坐标
extern float R_U_corner_angle;     // 右上拐点角度

extern uint8_t R_D_corner_flag;    // 右下拐点标志
extern uint8_t R_D_corner_row;     // 右下拐点行坐标
extern uint8_t R_D_corner_col;     // 右下拐点列坐标
extern float R_D_corner_angle;     // 右下拐点角度

extern uint8 turn_point_num;             // 拐点数量


void image_process(void);


#endif /* CODE_IMAGE_H_ */
