/*
 * Screen.c
 *
 *  Created on: 2025年11月25日
 *      Author: zhigu
 */




#include "Screen.h"









void draw_rec(uint8 x, uint8 y, const uint16 color)
{
    if(x < 6) x += 6;
    if(y < 6) y += 6;
    ips200_draw_line(x - 5, y - 5, x + 5, y - 5, color);
    ips200_draw_line(x - 5, y + 5, x + 5, y + 5, color);
    ips200_draw_line(x - 5, y - 5, x - 5, y + 5, color);
    ips200_draw_line(x + 5, y - 5, x + 5, y + 5, color);
}



//
//
////-------------------------------------------------------------------------------------------------------------------
//// 函数简介     显示油门控制进度条
//// 参数说明     x           起始x坐标
//// 参数说明     y           起始y坐标
//// 参数说明     width       进度条宽度
//// 参数说明     height      进度条高度
//// 参数说明     throttle    油门值 (-100 到 100)
//// 返回参数     void
//// 使用示例     ips200_show_throttle_bar(10, 10, 200, 20, 50);
//// 备注信息     进度条从中心向两侧填充，正值绿色，负值红色，背景白色
////-------------------------------------------------------------------------------------------------------------------
//void ips200_show_throttle_bar(uint16 x, uint16 y, uint16 width, uint16 height, int8 throttle)
//{
//    static int8 last_throttle = 0;  // 记录上次油门值，避免闪烁
//    static uint8 is_initialized = 0; // 记录是否已初始化背景和边框
//    static uint16 last_y = 0; // 记录上次y位置
//    zf_assert(x < ips200_x_max);
//    zf_assert(y < ips200_y_max);
//    zf_assert(x + width <= ips200_x_max);
//    zf_assert(y + height <= ips200_y_max);
//    zf_assert(throttle >= -100 && throttle <= 100);
//
//    uint16 bg_color = RGB565_WHITE;    // 背景色：白色
//    uint16 pos_color = RGB565_GREEN;   // 正半轴色：绿色
//    uint16 neg_color = RGB565_RED;     // 负半轴色：红色
//
//    uint16 center_x = x + width / 2;
//    uint16 half_width = width / 2;
//
//    // 如果未初始化或y位置变化，绘制背景和外围黑框
//    if(!is_initialized || y != last_y)
//    {
//        if(IPS200_TYPE_SPI == ips200_display_type)
//        {
//            IPS200_CS(0);
//        }
//        // 绘制内部背景
//        ips200_draw_rect(x, y, x + width - 1, y + height - 1, bg_color);
//        // 绘制外围黑框（如果坐标允许）
//        if(x > 0) ips200_draw_line(x - 1, y - 1, x - 1, y + height, RGB565_BLACK); // 左边
//        if(y > 0) ips200_draw_line(x - 1, y - 1, x + width, y - 1, RGB565_BLACK); // 上边
//        if(x + width < ips200_x_max) ips200_draw_line(x + width, y - 1, x + width, y + height, RGB565_BLACK); // 右边
//        if(y + height < ips200_y_max) ips200_draw_line(x - 1, y + height, x + width, y + height, RGB565_BLACK); // 下边
//        is_initialized = 1;
//        last_y = y;
//        last_throttle = 0; // 重置上次油门值，因为背景重新初始化
//    }
//
//    // 计算当前进度条宽度
//    int16 current_offset = (int16)((int32)half_width * throttle / 100);
//    if(throttle > 0 && current_offset == 0) current_offset = 1;
//    else if(throttle < 0 && current_offset == 0) current_offset = -1;
//    int16 last_offset = (int16)((int32)half_width * last_throttle / 100);
//
//    if(throttle == last_throttle)
//    {
//        return;
//    }
//
//    if(throttle > 0)
//    {
//        // 正值：绘制绿色条从中心到右
//        uint16 end_x = center_x + current_offset;
//        if(end_x > x + width - 1) end_x = x + width - 1;
//
//        if(current_offset > last_offset)
//        {
//            // 增加，绘制新增部分
//            if(end_x > center_x + last_offset)
//            {
//                ips200_draw_rect(center_x + last_offset, y, end_x - 1, y + height - 1, pos_color);
//            }
//        }
//        else if(current_offset < last_offset)
//        {
//            // 减少，清除旧部分为背景色
//            if(center_x + last_offset > center_x + current_offset)
//            {
//                ips200_draw_rect(center_x + current_offset, y, center_x + last_offset - 1, y + height - 1, bg_color);
//            }
//        }
//    }
//    else if(throttle < 0)
//    {
//        // 负值：绘制红色条从中心到左
//        uint16 start_x = center_x + current_offset;
//        if(start_x < x) start_x = x;
//
//        if(current_offset < last_offset)
//        {
//            // 更负，绘制新增部分
//            if(start_x < center_x + last_offset)
//            {
//                ips200_draw_rect(start_x, y, center_x + last_offset - 1, y + height - 1, neg_color);
//            }
//        }
//        else if(current_offset > last_offset)
//        {
//            // 减少负值，清除旧部分为背景色
//            if(center_x + last_offset < center_x + current_offset)
//            {
//                ips200_draw_rect(center_x + last_offset, y, center_x + current_offset - 1, y + height - 1, bg_color);
//            }
//        }
//    }
//    else
//    {
//        // throttle == 0，清除所有进度条
//        if(last_throttle != 0)
//        {
//            ips200_draw_rect(x, y, x + width - 1, y + height - 1, bg_color);
//        }
//        // 绘制中心指示线，表示0位置
//        ips200_draw_line(center_x, y, center_x, y + height - 1, RGB565_BLACK);
//    }
//
//    last_throttle = throttle;
//}
//
//
//
//
//
//
//
//
//
