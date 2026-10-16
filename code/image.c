/*
 * image.c
 *
 *  Created on: 2025年10月1日
 *      Author: zhigu
 */


#include "image.h"
#include "car_control.h"
#include "shared.h"


#define WHITE  255
#define BLACK  0



// 定义并初始化全局的图像状态标志实例
//Image_Status_Flag_t Image_Flag = {0}; // 将所有成员初始化为0/

uint8 Threshold;                          //大津法阈值
uint8 base_image[MT9V03X_H][MT9V03X_W];      // 复制图片 (120×180)
uint8 bin_image[MT9V03X_H][MT9V03X_W];      // 二值化图像 (120×180)


uint8 Mid_Line[MT9V03X_H];      // 中线
uint8 End_Mid_Line[MT9V03X_H];  // 滤波后中线




//===========================================大津法
/*-------------------------------------------------------------------------------------------------------------------
  @brief     大津法求阈值
  @param     image       图像数组
             col         列
             row         行
  @return    threshold   返回int阈值数值
  Sample     threshold=otsuThreshold(mt9v03x_image[0],MT9V03X_W, MT9V03X_H);
  @note      no
-------------------------------------------------------------------------------------------------------------------*/
uint8 otsuThreshold(uint8 *image, uint16 col, uint16 row)
{
#define GrayScale 256
    uint16 Image_Width  = col;
    uint16 Image_Height = row;
    int X;
    uint16 Y;
    uint8* data = image;
    int HistGram[GrayScale] = {0};

    uint32 Amount = 0;
    uint32 PixelBack = 0;
    uint32 PixelIntegralBack = 0;
    uint32 PixelIntegral = 0;
    int32 PixelIntegralFore = 0;
    int32 PixelFore = 0;
    double OmegaBack=0, OmegaFore=0, MicroBack=0, MicroFore=0, SigmaB=0, Sigma=0; // 类间方差;
    uint8 MinValue=0, MaxValue=0;
    uint8 Threshold = 0;

    for (Y = 0; Y <Image_Height; Y++) //Y<Image_Height改为Y =Image_Height；以便进行 行二值化
    {
        //Y=Image_Height;
        for (X = 0; X < Image_Width; X++)
        {
            HistGram[(int)data[Y*Image_Width + X]]++; //统计每个灰度值的个数信息
        }
    }

    for (MinValue = 0; MinValue < 256 && HistGram[MinValue] == 0; MinValue++) ;        //获取最小灰度的值
    for (MaxValue = 255; MaxValue > MinValue && HistGram[MinValue] == 0; MaxValue--) ; //获取最大灰度的值

    if (MaxValue == MinValue)
    {
        return MaxValue;          // 图像中只有一个颜色
    }
    if (MinValue + 1 == MaxValue)
    {
        return MinValue;      // 图像中只有二个颜色
    }

    for (Y = MinValue; Y <= MaxValue; Y++)
    {
        Amount += HistGram[Y];        //  像素总数
    }

    PixelIntegral = 0;
    for (Y = MinValue; Y <= MaxValue; Y++)
    {
        PixelIntegral += HistGram[Y] * Y;//灰度值总数
    }
    SigmaB = -1;
    for (Y = MinValue; Y < MaxValue; Y++)
    {
        PixelBack = PixelBack + HistGram[Y];    //前景像素点数
        PixelFore = Amount - PixelBack;         //背景像素点数
        OmegaBack = (double)PixelBack / Amount;//前景像素百分比
        OmegaFore = (double)PixelFore / Amount;//背景像素百分比
        PixelIntegralBack += HistGram[Y] * Y;  //前景灰度值
        PixelIntegralFore = PixelIntegral - PixelIntegralBack;//背景灰度值
        MicroBack = (double)PixelIntegralBack / PixelBack;//前景灰度百分比
        MicroFore = (double)PixelIntegralFore / PixelFore;//背景灰度百分比
        Sigma = OmegaBack * OmegaFore * (MicroBack - MicroFore) * (MicroBack - MicroFore);//g
        if (Sigma > SigmaB)//遍历最大的类间方差g
        {
            SigmaB = Sigma;
            Threshold = (uint8)Y;
        }
    }
    return Threshold;
}


//=========================================二值化
/*-------------------------------------------------------------------------------------------------------------------
  @brief     图像二值化处理函数
  @return    二值化之后的图像数组
  Sample     图像二值化
  @note      二值化处理，0黑，255是白，
-------------------------------------------------------------------------------------------------------------------*/
void turn_to_bin(void)
{
    uint8 i, j;
    //Threshold = otsuThreshold(mt9v03x_image[0], MT9V03X_W, MT9V03X_H);

    for(i = 0; i <40; i++)
    {
        for(j = 0; j < MT9V03X_W; j++)
        {
            if(base_image[i][j] > Threshold+20) bin_image[i][j] = WHITE;
            else bin_image[i][j] = BLACK;
        }
    }
    for(i = 40; i < MT9V03X_H; i++)
       {
           for(j = 0; j < MT9V03X_W; j++)
           {
               if(base_image[i][j] > Threshold) bin_image[i][j] = WHITE;
               else bin_image[i][j] = BLACK;
           }
       }
}
// in image.c

//=========================================二值化
/*-------------------------------------------------------------------------------------------------------------------
  @brief     图像二值化处理函数
  @return    二值化之后的图像数组
  Sample     图像二值化
  @note      二值化处理,0黑,255是白, 同时屏蔽左上角和右上角的三角形区域
-------------------------------------------------------------------------------------------------------------------*/
//void turn_to_bin(void)
//{
//    uint8 i, j;
//
//    // 定义左上角倒三角形的区域参数
//    const uint8 top_x_max = 70; // 最上边横坐标最大值
//    const uint8 left_y_max = 70; // 最左边纵坐标最大值
//
//    // 遍历图像的每一行
//    for(i = 0; i < MT9V03X_H; i++)
//    {
//        // 遍历图像的每一列
//        for(j = 0; j < MT9V03X_W; j++)
//        {
//            // --- 判断左上角倒三角形区域 ---
//            // 三角形的斜边方程可以简化为：y / left_y_max + x / top_x_max <= 1
//            // 为了避免浮点运算，转换为整数运算：y * top_x_max + x * left_y_max <= top_x_max * left_y_max
//            if ((uint32)i * top_x_max + (uint32)j * left_y_max <= (uint32)top_x_max * left_y_max)
//            {
//                bin_image[i][j] = BLACK;
//                continue; // 处理完毕，跳过该点，继续处理下一个点
//            }
//
//            // --- 判断右上角倒三角形区域 ---
//            // 右侧对称，斜边方程可以简化为：y / left_y_max + (image_w - 1 - x) / top_x_max <= 1
//            // 转换为整数运算：y * top_x_max + (image_w - 1 - x) * left_y_max <= top_x_max * left_y_max
//            if ((uint32)i * top_x_max + (uint32)(MT9V03X_W - 1 - j) * left_y_max <= (uint32)top_x_max * left_y_max)
//            {
//                bin_image[i][j] = BLACK;
//                continue; // 处理完毕，跳过该点
//            }
//
//            // --- 如果不在屏蔽区域内，则执行正常的二值化 ---
//            if(i < 40) // 图像上部
//            {
//                if(base_image[i][j] > Threshold + 5)
//                    bin_image[i][j] = WHITE;
//                else
//                    bin_image[i][j] = BLACK;
//            }
//            else // 图像下部
//            {
//                if(base_image[i][j] > Threshold)
//                    bin_image[i][j] = WHITE;
//                else
//                    bin_image[i][j] = BLACK;
//            }
//        }
//    }
//}

//=========================================//形态学滤波
//void image_filter(uint8(*bin_image)[MT9V03X_W])
//{
//    uint16 i, j;
//    uint32 num = 0;
//    uint16 threshold_max = WHITE * 7;
//    uint16 threshold_min = WHITE * 3;
//
//    // 创建临时缓冲区，存储原始图像
//    uint8 temp_image[MT9V03X_H][MT9V03X_W];
//
//    // 复制原始图像到临时缓冲区
//    for (i = 0; i < MT9V03X_H; i++) {
//        for (j = 0; j < MT9V03X_W; j++) {
//            temp_image[i][j] = bin_image[i][j];
//        }
//    }
//
//    // 处理时读取临时缓冲区（原始数据），写入原缓冲区
//    for (i = 1; i < MT9V03X_H - 1; i++) {
//        for (j = 1; j < MT9V03X_W - 1; j++) {
//            // 从临时缓冲区读取邻域信息（保证是原始数据）
//            num = temp_image[i-1][j-1] + temp_image[i-1][j] + temp_image[i-1][j+1]
//                + temp_image[i][j-1] + temp_image[i][j+1]
//                + temp_image[i+1][j-1] + temp_image[i+1][j] + temp_image[i+1][j+1];
//
//            // 决策基于原始数据，修改当前缓冲区
//            if (num >= threshold_max && temp_image[i][j] == BLACK) {
//                bin_image[i][j] = WHITE; // 修改当前图像
//            }
//            else if (num <= threshold_min && temp_image[i][j] == WHITE) {
//                bin_image[i][j] = BLACK;
//            }
//        }
//    }
//}

void image_filter(uint8(*bin_image)[MT9V03X_W])//形态学滤波，简单来说就是膨胀和腐蚀的思想
{
   uint16 i, j;
   uint32 num = 0;
   uint16 threshold_max = WHITE * 7;
   uint16 threshold_min = WHITE * 3;
    for (i = 1; i < MT9V03X_H - 1; i++)
    {
        for (j = 1; j < (MT9V03X_W - 1); j++)
        {
            //统计八个方向的像素值
            num =
                bin_image[i - 1][j - 1] + bin_image[i - 1][j] + bin_image[i - 1][j + 1]
                + bin_image[i][j - 1] + bin_image[i][j + 1]
                + bin_image[i + 1][j - 1] + bin_image[i + 1][j] + bin_image[i + 1][j + 1];

            if (num >= threshold_max && bin_image[i][j] == BLACK)
            {

                bin_image[i][j] = WHITE;//白  可以搞成宏定义，方便更改
            }
            if (num <= threshold_min && bin_image[i][j] == WHITE)
            {
                bin_image[i][j] = BLACK;//黑
            }

        }
    }

}

//===================================================//绘制矩形框
/**
 * 在图像上绘制矩形框。
 * 该函数通过将图像数组的边缘像素点设置为0，从而在图像上绘制一个矩形框。
 *
 * @param image 指向图像数组的指针，图像数组的类型为uint8，大小为MT9V03X_W。
 *              图像数组由调用该函数的外部程序提供。
 */
void image_draw_rectan(uint8(*image)[MT9V03X_W])
{

    // 初始化循环变量i为0。
    uint8 i = 0;

    // 清除图像数组的左侧和右侧边缘像素点。
    for (i = 0; i < MT9V03X_H - 1; i++)
    {
        image[i][0] = 0;
        image[i][1] = 0;
        image[i][MT9V03X_W - 1] = 0;
        image[i][MT9V03X_W - 2] = 0;
    }

    // 清除图像数组的顶部和底部边缘像素点。
    for (i = 0; i < MT9V03X_W - 1; i++)
    {
        image[0][i] = 0;
        image[1][i] = 0;
        image[2][i] = 0;
        image[3][i] = 0;
    //    image[MT9V03X_H-1][i] = 0;
    }
}

//========================================================功能函数：绝对值、限幅

int my_abs(int value)
{
if(value>=0) return value;
else return -value;
}


int16 limit_a_b(int16 x, int16 a, int16 b)
{
    if(x<a) x = a;
    if(x>b) x = b;
    return x;
}

int16 limit1(int16 x, int16 y)
{
    if (x > y)             return y;
    else if (x < -y)       return -y;
    else                return x;
}

//=========================================================  get_start_point

/*
函数名称：void get_start_point(uint8 start_row)
功能说明：寻找两个边界的边界点作为八邻域循环的起始点
参数说明：输入任意行数
函数返回：无
备    注：
example：  get_start_point(image_h-2)
 */
uint8 start_point_l[2] = { 0 };//左边起点的x，y值
uint8 start_point_r[2] = { 0 };//右边起点的x，y值
uint8 get_start_point(uint8 start_row)
{
    uint8 i = 0,l_found = 0,r_found = 0;
    //清零
    start_point_l[0] = 0;//x
    start_point_l[1] = 0;//y

    start_point_r[0] = 0;//x
    start_point_r[1] = 0;//y

        //从中间往左边，先找起点
    for (i = image_w / 2; i > border_min; i--)
    {
        start_point_l[0] = i;//x
        start_point_l[1] = start_row;//y
        if (bin_image[start_row][i] == 255 && bin_image[start_row][i - 1] == 0)//当前是白色，左侧是黑色
        {
            //printf("找到左边起点image[%d][%d]\n", start_row,i);
            l_found = 1;
            break;
        }
    }

    for (i = image_w / 2; i < border_max; i++)
    {
        start_point_r[0] = i;//x
        start_point_r[1] = start_row;//y
        if (bin_image[start_row][i] == 255 && bin_image[start_row][i + 1] == 0)
        {
            //printf("找到右边起点image[%d][%d]\n",start_row, i);
            r_found = 1;
            break;
        }
    }

    if(l_found&&r_found)return 1;
    else {
        //printf("未找到起点\n");
        return 0;
    }
}

/*
函数名称：void search_l_r(uint16 break_flag, uint8(*image)[image_w],uint16 *l_stastic, uint16 *r_stastic,
                            uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y,uint8*hightest)

功能说明：八邻域正式开始找右边点的函数，输入参数有点多，调用的时候不要漏了，这个是左右线一次性找完。
参数说明：
break_flag_r            ：最多需要循环的次数
(*image)[image_w]       ：需要进行找点的图像数组，必须是二值图,填入数组名称即可
                       特别注意，不要拿宏定义名字作为输入参数，否则数据可能无法传递过来
*l_stastic              ：统计左边数据，用来输入初始数组成员的序号和取出循环次数
*r_stastic              ：统计右边数据，用来输入初始数组成员的序号和取出循环次数
l_start_x               ：左边起点横坐标
l_start_y               ：左边起点纵坐标
r_start_x               ：右边起点横坐标
r_start_y               ：右边起点纵坐标
hightest                ：循环结束所得到的最高高度
函数返回：无
备    注：
example：
    search_l_r((uint16)USE_num,image,&data_stastics_l, &data_stastics_r,start_point_l[0],
                start_point_l[1], start_point_r[0], start_point_r[1],&hightest);
 */
#define USE_num image_h*3   //定义找点的数组成员个数按理说300个点能放下，但是有些特殊情况确实难顶，多定义了一点

 //存放点的x，y坐标
uint16 points_l[(uint16)USE_num][2] = { {  0 } };//左线
uint16 points_r[(uint16)USE_num][2] = { {  0 } };//右线
uint16 dir_r[(uint16)USE_num] = { 0 };//用来存储右边生长方向
uint16 dir_l[(uint16)USE_num] = { 0 };//用来存储左边生长方向
uint16 data_stastics_l = 0;//统计左边找到点的个数
uint16 data_stastics_r = 0;//统计右边找到点的个数
uint8 hightest = 0;//最高点

void search_l_r(uint16 break_flag, uint8(*image)[image_w], uint16 *l_stastic, uint16 *r_stastic, uint8 l_start_x, uint8 l_start_y, uint8 r_start_x, uint8 r_start_y, uint8*hightest)
{

    uint8 i = 0, j = 0;
    //左边变量
    uint8 search_filds_l[8][2] = { {  0 } };
    uint8 index_l = 0;
    uint8 temp_l[8][2] = { {  0 } };
    uint8 center_point_l[2] = {  0 };
    uint16 l_data_statics;//统计左边
    //定义八个邻域
    static int8 seeds_l[8][2] = { {0,  1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,  0},{1, 1}, };
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //这个是顺时针

    //右边变量
    uint8 search_filds_r[8][2] = { {  0 } };
    uint8 center_point_r[2] = { 0 };//中心坐标点
    uint8 index_r = 0;
    uint8 temp_r[8][2] = { {  0 } };
    uint16 r_data_statics;//统计右边
    //定义八个邻域
    static int8 seeds_r[8][2] = { {0,  1},{1,1},{1,0}, {1,-1},{0,-1},{-1,-1}, {-1,  0},{-1, 1}, };
    //{-1,-1},{0,-1},{+1,-1},
    //{-1, 0},       {+1, 0},
    //{-1,+1},{0,+1},{+1,+1},
    //这个是逆时针

    l_data_statics = *l_stastic;//统计找到了多少个点，方便后续把点全部画出来
    r_data_statics = *r_stastic;//统计找到了多少个点，方便后续把点全部画出来

    //第一次更新坐标点  将找到的起点值传进来
    center_point_l[0] = l_start_x;//x
    center_point_l[1] = l_start_y;//y
    center_point_r[0] = r_start_x;//x
    center_point_r[1] = r_start_y;//y

        //开启邻域循环
    while (break_flag--)
    {

        //左边
        for (i = 0; i < 8; i++)//传递8F坐标     一次传递八个值
        {
            search_filds_l[i][0] = center_point_l[0] + seeds_l[i][0];//x           //中心坐标加减单位向量坐标
            search_filds_l[i][1] = center_point_l[1] + seeds_l[i][1];//y
        }
        //中心坐标点填充到已经找到的点内
        points_l[l_data_statics][0] = center_point_l[0];//x
        points_l[l_data_statics][1] = center_point_l[1];//y
        l_data_statics++;//索引加一

        //右边
        for (i = 0; i < 8; i++)//传递8F坐标
        {
            search_filds_r[i][0] = center_point_r[0] + seeds_r[i][0];//x
            search_filds_r[i][1] = center_point_r[1] + seeds_r[i][1];//y
        }
        //中心坐标点填充到已经找到的点内
        points_r[r_data_statics][0] = center_point_r[0];//x
        points_r[r_data_statics][1] = center_point_r[1];//y


        //======================

        index_l = 0;//先清零，后使用
        for (i = 0; i < 8; i++)
        {
            temp_l[i][0] = 0;//先清零，后使用
            temp_l[i][1] = 0;//先清零，后使用
        }

        //左边判断
        for (i = 0; i < 8; i++)
        {
            if (image[search_filds_l[i][1]][search_filds_l[i][0]] == 0
                && image[search_filds_l[(i + 1) & 7][1]][search_filds_l[(i + 1) & 7][0]] == 255)     //  (i + 1) & 7  （加一再对8取余数，更高效的方式）
            {
                temp_l[index_l][0] = search_filds_l[(i)][0];
                temp_l[index_l][1] = search_filds_l[(i)][1];
                index_l++;
                dir_l[l_data_statics - 1] = (i);//记录生长方向
            }

            if (index_l)
            {
                //更新坐标点
                center_point_l[0] = temp_l[0][0];//x
                center_point_l[1] = temp_l[0][1];//y
                for (j = 0; j < index_l; j++)
                {
                    if (center_point_l[1] > temp_l[j][1])//y
                    {
                        center_point_l[0] = temp_l[j][0];//x
                        center_point_l[1] = temp_l[j][1];//y
                    }
                }
            }

        }
        if ((points_r[r_data_statics][0]== points_r[r_data_statics-1][0]&& points_r[r_data_statics][0] == points_r[r_data_statics - 2][0]
            && points_r[r_data_statics][1] == points_r[r_data_statics - 1][1] && points_r[r_data_statics][1] == points_r[r_data_statics - 2][1])
            ||(points_l[l_data_statics-1][0] == points_l[l_data_statics - 2][0] && points_l[l_data_statics-1][0] == points_l[l_data_statics - 3][0]
                && points_l[l_data_statics-1][1] == points_l[l_data_statics - 2][1] && points_l[l_data_statics-1][1] == points_l[l_data_statics - 3][1]))
        {
            //printf("三次进入同一个点，退出\n");
            break;
        }
        if (my_abs(points_r[r_data_statics][0] - points_l[l_data_statics - 1][0]) < 2
            && my_abs(points_r[r_data_statics][1] - points_l[l_data_statics - 1][1] < 2)
            )
        {
           // printf("\n左右相遇退出\n");
            *hightest = (points_r[r_data_statics][1] + points_l[l_data_statics - 1][1]) >> 1;//取出最高点
            //printf("\n在y=%d处退出\n",*hightest);
            break;
        }
        if ((points_r[r_data_statics][1] < points_l[l_data_statics - 1][1]))
        {
           // printf("\n如果左边比右边高了，左边等待右边\n");
            continue;//如果左边比右边高了，左边等待右边
        }
        if (dir_l[l_data_statics - 1] == 7
            && (points_r[r_data_statics][1] > points_l[l_data_statics - 1][1]))//左边比右边高且已经向下生长了
        {
           // printf("\n左边开始向下了，等待右边，等待中... \n");
            center_point_l[0] = points_l[l_data_statics - 1][0];//x
            center_point_l[1] = points_l[l_data_statics - 1][1];//y
            l_data_statics--;
        }
        r_data_statics++;//索引加一

        index_r = 0;//先清零，后使用
        for (i = 0; i < 8; i++)
        {
            temp_r[i][0] = 0;//先清零，后使用
            temp_r[i][1] = 0;//先清零，后使用
        }

        //右边判断
        for (i = 0; i < 8; i++)
        {
            if (image[search_filds_r[i][1]][search_filds_r[i][0]] == 0
                && image[search_filds_r[(i + 1) & 7][1]][search_filds_r[(i + 1) & 7][0]] == 255)
            {
                temp_r[index_r][0] = search_filds_r[(i)][0];
                temp_r[index_r][1] = search_filds_r[(i)][1];
                index_r++;//索引加一
                dir_r[r_data_statics - 1] = (i);//记录生长方向
               // printf("dir[%d]:%d\n", r_data_statics - 1, dir_r[r_data_statics - 1]);
            }
            if (index_r)
            {

                //更新坐标点
                center_point_r[0] = temp_r[0][0];//x
                center_point_r[1] = temp_r[0][1];//y
                for (j = 0; j < index_r; j++)
                {
                    if (center_point_r[1] > temp_r[j][1])
                    {
                        center_point_r[0] = temp_r[j][0];//x
                        center_point_r[1] = temp_r[j][1];//y
                    }
                }

            }
        }
    }
    //取出循环次数
    *l_stastic = l_data_statics;
    *r_stastic = r_data_statics;
}

/*
函数名称：void get_left(uint16 total_L)
功能说明：从八邻域边界里提取需要的边线
参数说明：
total_L ：找到的点的总数
函数返回：无
备    注：
example： get_left(data_stastics_l );
 */
uint8 l_border[image_h];//左线数组
uint8 r_border[image_h];//右线数组
uint8 center_line[image_h];//中线数组

void get_left(uint16 total_L)
{
    uint8 i = 0;
    uint16 j = 0;

    // 初始化所有行为边界最小值
    for (i = 0; i < image_h; i++)
    {
        l_border[i] = border_min;
    }

    // 遍历所有找到的点，直接填充到对应行
    for (j = 0; j < total_L; j++)
    {
        uint8 row = points_l[j][1];  // 获取点的行坐标
        if (row < image_h) {
            l_border[row] = (uint8)points_l[j][0] + 1;  // 直接填充到对应行
        }
    }

    // 从底部向上插值填充缺失的行
    for (i = image_h - 2; i > 0; i--)
    {
        // 如果当前行没有有效数据，使用下一行的数据
        if (l_border[i] == border_min && l_border[i + 1] != border_min) {
            l_border[i] = (uint8)l_border[i + 1];
        }
    }
}

// 改进的 get_right 函数
void get_right(uint16 total_R)
{
    uint8 i = 0;
    uint16 j = 0;

    // 初始化所有行为边界最大值
    for (i = 0; i < image_h; i++)
    {
        r_border[i] = border_max;
    }

    // 遍历所有找到的点，直接填充到对应行
    for (j = 0; j < total_R; j++)
    {
        uint8 row = points_r[j][1];  // 获取点的行坐标
        if (row < image_h) {
            r_border[row] = points_r[j][0] - 1;  // 直接填充到对应行
        }
    }

    // 从底部向上插值填充缺失的行
    for (i = image_h - 2; i > 0; i--)
    {
        // 如果当前行没有有效数据，使用下一行的数据
        if (r_border[i] == border_max && r_border[i + 1] != border_max) {
            r_border[i] = r_border[i + 1];
        }
    }
}
/*-------------------------------------------------------------------------------------------------------------------
  @brief     画线
  @param     输入起始点，终点坐标，补一条宽度为2的黑线
  @return    null
  Sample     Draw_Line(0, 0,MT9V03X_W-1,MT9V03X_H-1);
             Draw_Line(MT9V03X_W-1, 0,0,MT9V03X_H-1);
                                    画一个大×
  @note     补的就是一条线，需要重新扫线
-------------------------------------------------------------------------------------------------------------------*/
void Draw_Line(int startX, int startY, int endX, int endY)
{
    int i,x,y;
    int start=0,end=0;
    if(startX>=MT9V03X_W-1)//限幅处理
        startX=MT9V03X_W-1;
    else if(startX<=0)
        startX=0;
    if(startY>=MT9V03X_H-1)
        startY=MT9V03X_H-1;
    else if(startY<=0)
        startY=0;
    if(endX>=MT9V03X_W-1)
        endX=MT9V03X_W-1;
    else if(endX<=0)
        endX=0;
    if(endY>=MT9V03X_H-1)
        endY=MT9V03X_H-1;
    else if(endY<=0)
        endY=0;
    if(startX==endX)//一条竖线
    {
        if (startY > endY)//互换
        {
            start=endY;
            end=startY;
        }
        for (i = start; i <= end; i++)
        {
            if(i<=2)
                i=2;
            bin_image[i][startX]=BLACK;
            bin_image[i-1][startX]=BLACK;
            bin_image[i-2][startX]=BLACK;

        }
    }
    else if(startY == endY)//补一条横线
    {
        if (startX > endX)//互换
        {
            start=endX;
            end=startX;
        }
        for (i = start; i <= end; i++)
        {
            if(startY<=1)
                startY=2;
            bin_image[startY][i]=BLACK;
            bin_image[startY-1][i]=BLACK;
            bin_image[startY-2][i]=BLACK;
        }
    }
    else //上面两个是水平，竖直特殊情况，下面是常见情况
    {
        if(startY>endY)//起始点矫正
        {
            start=endY;
            end=startY;
        }
        else
        {
            start=startY;
            end=endY;
        }
        for (i = start; i <= end; i++)//纵向补线，保证每一行都有黑点
        {
            x =(int)(startX+(endX-startX)*(i-startY)/(endY-startY));//两点式变形
            if(x>=MT9V03X_W-3)
                x=MT9V03X_W-3;
            else if (x<=3)
                x=3;
            bin_image[i][x] = BLACK;
            bin_image[i][x-1] = BLACK;
            bin_image[i][x-2] = BLACK;
            bin_image[i][x-3] = BLACK;
        }
        if(startX>endX)
        {
            start=endX;
            end=startX;
        }
        else
        {
            start=startX;
            end=endX;
        }
        for (i = start; i <= end; i++)//横向补线，保证每一列都有黑点
        {

            y =(int)(startY+(endY-startY)*(i-startX)/(endX-startX));//两点式变形
            if(y>=MT9V03X_H-1)
                y=MT9V03X_H-1;
            else if (y<=0)
                y=0;
            bin_image[y][i] = BLACK;
        }
    }
}

//==================================================================================================
/**
 * get_lost_line函数用于检测并统计在图像中丢失的线条信息
 * 该函数没有输入参数和返回值
 *
 * 本函数首先从图像的底部开始向上遍历，检查左右两边的线条数据
 * 如果检测到左边的线条数据为1，则设置相应的丢失标志
 * 如果检测到右边的线条数据为138，则设置相应的丢失标志
 *
 * 接着，再次从图像底部开始向上遍历，根据之前设置的丢失标志进行统计
 * 统计左边、右边以及双边同时丢失线条的数量
 */


uint8_t Left_Lost_Time=0;
uint8_t Right_Lost_Time=0;
uint8_t Both_Lost_Time=0;

// 丢失检测相关数组
uint8_t Left_Lost_Flag[MT9V03X_H] = {0};
uint8_t Right_Lost_Flag[MT9V03X_H] = {0};




uint8_t get_lost_line(uint8 *Left_Line,uint8 *Right_Line)
{
    uint16 i;

    Left_Lost_Time=0;
    Right_Lost_Time=0;
    Both_Lost_Time=0;

    for(i =0; i <= MT9V03X_H-1; i++)                //将竖向数据清零
    {
        Left_Lost_Flag[i] = 0;
        Right_Lost_Flag[i] =0;
    }

    // 从图像底部开始向上遍历，检查并标记丢失的线条
//  for(i = 0; i <= data_stastics_l - 1; i++)
//  {
//    if(points_l[i][0] <=2)
//    {
//      Left_Lost_Flag[i] = 1;
//          Left_Lost_Time++;
//    }
//  }
//  for(i = 0; i <= data_stastics_r-1; i++)
//  {
//    if(points_r[i][0] >=MT9V03X_W-3)
//    {
//      Right_Lost_Flag[i] = 1;
//          Right_Lost_Time++;
//    }
//  }

    for(i = 1; i < MT9V03X_H - 2; i++)
    {
        if(Left_Line[i] <= 2)
        {
            Left_Lost_Flag[i] = 1;
            Left_Lost_Time++;
        }

        if(Right_Line[i] >= MT9V03X_W - 3)
        {
            Right_Lost_Flag[i] = 1;
            Right_Lost_Time++;
        }
    }
    // 赛道数据初步分析，统计左右两边及双边同时丢失线条的数量
    for (i = MT9V03X_H - 1; i > 1; i--)
    {
//    if (Left_Lost_Flag[i]  == 1)//单边丢线数
//      Left_Lost_Time++;
//    if (Right_Lost_Flag[i] == 1)
//      Right_Lost_Time++;
        if (Left_Lost_Flag[i] == 1 && Right_Lost_Flag[i] == 1)//双边丢线数
            Both_Lost_Time++;
    }

    if((Left_Lost_Time>30)&&Right_Lost_Time>30)
    {
        return 1;
    }
    else if(Left_Lost_Time>30)
    {
        return 2;
    }
    else if(Right_Lost_Time>30)
    {
        return 3;
    }
    else
    {
        return 0;
    }
}
//============================================================================拐点检测


uint8_t L_U_corner_flag = 0;
uint8_t L_U_corner_row = 0;
uint8_t L_U_corner_col = 0;
float L_U_corner_angle = 0.0f;

uint8_t L_D_corner_flag = 0;
uint8_t L_D_corner_row = 0;
uint8_t L_D_corner_col = 0;
float L_D_corner_angle = 0.0f;

uint8_t R_U_corner_flag = 0;
uint8_t R_U_corner_row = 0;
uint8_t R_U_corner_col = 0;
float R_U_corner_angle = 0.0f;

uint8_t R_D_corner_flag = 0;
uint8_t R_D_corner_row = 0;
uint8_t R_D_corner_col = 0;
float R_D_corner_angle = 0.0f;

uint8 turn_point_num = 0;             // 拐点数量
uint8 break_flag = 0;                 // 调试标志


void get_turning_point(uint8* Left_Line, uint8* Right_Line, uint16 image_height)
{
    uint16_t i;
    break_flag=0;
    L_U_corner_flag = 0;
    L_U_corner_row = 0;
    L_U_corner_col = 0;

    L_D_corner_flag = 0;
    L_D_corner_row = 0;
    L_D_corner_col = 0;

    R_U_corner_flag = 0;
    R_U_corner_row = 0;
    R_U_corner_col = 0;

    R_D_corner_flag = 0;
    R_D_corner_row = 0;
    R_D_corner_col = 0;

    for (i = 20; i < MT9V03X_H / 2-20; i++)//寻找左上拐点
    {
        if(Left_Line[i]<Left_Line[i-10])//上面的值大于该点的值，避免圆环中拐点
        {

            if ((abs(Left_Line[i] - Left_Line[i - 1]) <= 5)
                    && (abs(Left_Line[i - 1] - Left_Line[i - 2]) <= 5)
                    && (abs(Left_Line[i - 2] - Left_Line[i - 3]) <= 3)
                    && (Left_Line[i] - Left_Line[i + 3] >= 8)
                    &&(Left_Line[i+2] - Left_Line[i + 3] <=3))
            {
                L_U_corner_row = i;//传递y坐标
                L_U_corner_col = (uint8)Left_Line[i];
                L_U_corner_flag = 1;
                break;
            }
        }
    }

    for (i =  20; i < MT9V03X_H / 2-20; i++)//寻找右上拐点
    {
        if(Right_Line[i]>Right_Line[i-10])
        {
            if ((abs(Right_Line[i] - Right_Line[i - 1]) <= 5)
                    && (abs(Right_Line[i - 1] - Right_Line[i - 2]) <= 5)
                    && (abs(Right_Line[i - 2] - Right_Line[i - 3]) <= 3)
                    && (Right_Line[i + 3] - Right_Line[i] >= 8)
                    && (Right_Line[i+2] - Right_Line[i + 3] <=3))
            {
                R_U_corner_row = i;//传递y坐标
                R_U_corner_col =(uint8) Right_Line[i];
                R_U_corner_flag = 1;
                break;
            }
        }
    }

    for (i = MT9V03X_H - 15; i > MT9V03X_H / 2 - 20; i--)//寻找左下拐点
    {

        if ((abs(Left_Line[i] - Left_Line[i + 1]) <= 5)
                && (abs(Left_Line[i + 1] - Left_Line[i + 2]) <= 5)
                && (abs(Left_Line[i + 2] - Left_Line[i + 3]) <= 3)
                && (Left_Line[i] - Left_Line[i - 3] >= 8))
        {

            L_D_corner_row = i;//传递y坐标
            L_D_corner_col =(uint8) Left_Line[i];
            L_D_corner_flag = 1;
            break;
        }
    }

    for (i = MT9V03X_H - 15; i > MT9V03X_H / 2 - 20; i--)//寻找右下拐点
    {

        if ((abs(Right_Line[i] - Right_Line[i + 1]) <= 5)
                && (abs(Right_Line[i + 1] - Right_Line[i + 2]) <= 5)
                && (abs(Right_Line[i + 2] - Right_Line[i + 3]) <= 3)
                && (Right_Line[i - 3] - Right_Line[i] >= 8))
        {
            R_D_corner_row = i;//传递y坐标
            R_D_corner_col =(uint8) Right_Line[i];
            R_D_corner_flag = 1;
            break;
        }
    }

    if ((R_D_corner_row - R_U_corner_row < 0) && (R_D_corner_row) && (R_U_corner_row))
    {
        R_D_corner_row = 0;
        R_D_corner_col = 0;
        R_D_corner_flag =0;
    }
    if ((L_D_corner_row - L_U_corner_row < 0) && (L_D_corner_row) && (L_U_corner_row))
    {
        L_D_corner_row = 0;
        L_D_corner_col = 0;
        L_D_corner_flag = 0;
    }
    if ((R_U_corner_row - R_D_corner_row > 0) && (R_D_corner_row) && (R_U_corner_row))
    {
        R_U_corner_row = 0;
        R_U_corner_col = 0;
        R_U_corner_flag = 0;
    }
    if ((L_U_corner_row - L_D_corner_row > 0) && (L_D_corner_row) && (L_U_corner_row))
    {
        L_U_corner_row = 0;
        L_U_corner_col = 0;
        L_U_corner_flag = 0;
    }

    turn_point_num=0;
    if(L_U_corner_flag ==1) turn_point_num ++;
    if(R_U_corner_flag ==1) turn_point_num ++;
    if(L_D_corner_flag ==1) turn_point_num ++;
    if(R_D_corner_flag ==1) turn_point_num ++;

}
//======================================================================================计算斜率
/**
 * @brief      根据两点坐标计算直线的斜率和截距
 * @param      x1 第一个点的x坐标(列)
 * @param      y1 第一个点的y坐标(行)
 * @param      x2 第二个点的x坐标(列)
 * @param      y2 第二个点的y坐标(行)
 * @param      slope 指向斜率存储变量的指针 (输出参数)
 * @param      intercept 指向截距存储变量的指针 (输出参数)
 * @return     无
 * @note       计算的是 x = slope * y + intercept 形式的直线方程,
 *             用于根据行(y)计算列(x)。
 */
#include <math.h>  // 用于 fabs() 等数学函数，如果需要限幅

// 计算两点 (col1, row1) 和 (col2, row2) 确定的直线斜率 k 和截距 b
// 输入：col1, row1 - 第一点坐标 (x1, y1)
//       col2, row2 - 第二点坐标 (x2, y2)
// 输出：*slope - 斜率 k = (y2 - y1) / (x2 - x1)
//       *intercept - 截距 b = y1 - k * x1
// 注意：
// - 如果 col1 == col2 (垂直线)，设置 slope=0, intercept=平均 y 值（近似处理，避免除零）
// - 如果 row1 == row2 (水平线)，slope=0, intercept=row1
// - 自动处理点顺序颠倒（确保 Δx >=0 以避免负斜率符号问题）
// - 类型安全：uint8_t 显式转换为 float，避免截断警告
//void Get_K_b(uint8_t col1, uint8_t row1, uint8_t col2, uint8_t row2, float *slope, float *intercept)
//{
//    float x1 = (float)col1;   // 显式转换为 float，避免截断
//    float y1 = (float)row1;
//    float x2 = (float)col2;
//    float y2 = (float)row2;
//
//    float delta_x = x2 - x1;
//    float delta_y = y2 - y1;
//
//    // 边界检查：如果两点相同，直接返回原点值
//    if (fabs(delta_x) < 1e-6f && fabs(delta_y) < 1e-6f) {  // 浮点比较阈值
//        *slope = 0.0f;
//        *intercept = y1;
//        return;
//    }
//
//    // 处理垂直线 (delta_x ≈ 0)：近似为水平线，slope=0，intercept=平均 y
//    if (fabs(delta_x) < 1e-6f) {
//        *slope = 0.0f;
//        *intercept = (y1 + y2) / 2.0f;
//        return;
//    }
//
//    // 处理水平线 (delta_y ≈ 0)：slope=0
//    if (fabs(delta_y) < 1e-6f) {
//        *slope = 0.0f;
//        *intercept = y1;
//        return;
//    }
//
//    // 确保 delta_x > 0（交换点顺序，避免负斜率符号问题）
//    if (delta_x < 0.0f) {
//        float temp = x1; x1 = x2; x2 = temp;
//        temp = y1; y1 = y2; y2 = temp;
//        delta_x = -delta_x;
//        delta_y = -delta_y;
//    }
//
//    // 计算斜率 k = delta_y / delta_x
//    *slope = delta_y / delta_x;
//
//    // 计算截距 b = y1 - k * x1
//    *intercept = y1 - (*slope) * x1;
//
//    // 可选：限幅输出（防止极端值，根据您的图像尺寸 MT9V03X_W=188, MT9V03X_H=120 调整）
//    // 如果 slope 过大（陡峭线），可夹紧到合理范围，例如 [-10, 10]
//    if (*slope > 10.0f) *slope = 10.0f;
//    if (*slope < -10.0f) *slope = -10.0f;
//    // intercept 限幅到图像高度 [0, MT9V03X_H]
//    if (*intercept > (float)MT9V03X_H) *intercept = (float)MT9V03X_H;
//    if (*intercept < 0.0f) *intercept = 0.0f;
//}
void Get_K_b(float x1, float y1, float x2, float y2, float *slope, float *intercept)
{
    // 注意：这里传入的参数顺序是 (列坐标, 行坐标, 列坐标, 行坐标)
    // 但在图像坐标系中，通常行坐标是y，列坐标是x

    // 防止除零错误
    if (fabs(x2 - x1) < 0.0001f) {
        // 处理垂直线情况
        *slope = 100000.0f;  // 很大的斜率值
    } else {
        // 计算斜率：k = (y2 - y1) / (x2 - x1)
        *slope = (y2 - y1) / (x2 - x1);
    }

    // 计算截距：b = y1 - k * x1
    *intercept = y1 - (*slope) * x1;
}
//===========================================================================================十字

void Cross_fill(uint8* Left_Line, uint8* Right_Line)
{
    uint16 i;

    uint16 start, end;

    float slope_l_rate = 0, intercept_l = 0;

    if(get_lost_line(l_border, r_border) == 1)
    {
        if(turn_point_num >= 2)
        {
            Image_Flag.Cross_Fill =1;
        }
//        if(Image_Flag.Cross_Fill == true) Set_Beepfreq(1);

        if ((L_D_corner_flag) && (L_U_corner_flag) && (R_D_corner_flag) && (R_U_corner_flag))//同时找到四个拐点 1111
        {
            Image_Flag.Cross_Fill = 1;
            //计算斜率,左边斜率
            Get_K_b(Left_Line[L_D_corner_row], L_D_corner_row, Left_Line[L_U_corner_row],L_U_corner_row,&slope_l_rate,&intercept_l);
            for (i = L_D_corner_row; i > L_U_corner_row; i--)
            {
                Left_Line[i] = (i - intercept_l)/ slope_l_rate;//y = kx+b
                Left_Line[i] = (uint8)limit_a_b((int16_t)Left_Line[i], border_min, border_max);//限幅
            }

            //计算斜率,右边斜率
            Get_K_b(Right_Line[R_D_corner_row], R_D_corner_row, Right_Line[R_U_corner_row], R_U_corner_row, &slope_l_rate, &intercept_l);
            for (i = R_D_corner_row; i > R_U_corner_row; i--)
            {
                Right_Line[i] = (i - intercept_l) / slope_l_rate;//y = kx+b
                Right_Line[i] = (uint8)limit_a_b((int16_t)Right_Line[i], border_min, border_max);//限幅
            }
        }
        else if ((L_D_corner_flag) && (L_U_corner_flag) && (!R_D_corner_flag) && (R_U_corner_flag))//右斜入十字 1101
        {
            Image_Flag.Cross_Fill = 1;
            //计算斜率
            Get_K_b(Left_Line[L_U_corner_row], L_U_corner_row, Left_Line[L_D_corner_row], L_D_corner_row, &slope_l_rate, &intercept_l);
            for (i = L_D_corner_row; i > L_U_corner_row; i--)
            {
                Left_Line[i] = (i - intercept_l) / slope_l_rate;//y = kx+b
                Left_Line[i] = (uint8)limit_a_b((int16_t)Left_Line[i], border_min, border_max);//限幅
            }

            //计算斜率
            start = R_U_corner_row - 15;//起点
            start = limit_a_b(start, 5, MT9V03X_H-5);//限幅
            end = R_U_corner_row;//终点
            Get_K_b(Right_Line[start], start, Right_Line[end], end, &slope_l_rate, &intercept_l);
            for (i = R_U_corner_row; i < MT9V03X_H - 2; i++)
            {
                Right_Line[i] = (i - intercept_l) / slope_l_rate;//y = kx+b
                Right_Line[i] =(uint8)limit_a_b((int16_t)Right_Line[i], border_min, border_max);
            }
        }
        else if ((!L_D_corner_flag) && (L_U_corner_flag) && (R_U_corner_flag) && (R_D_corner_flag))//左斜入十字 0111
        {
            Image_Flag.Cross_Fill = 1;
            //计算斜率
            start = L_U_corner_row - 15;
            start = limit_a_b(start, 5, MT9V03X_H - 5);
            end = L_U_corner_row;
            Get_K_b(Left_Line[start], start, Left_Line[end], end, &slope_l_rate, &intercept_l);
            for (i = L_U_corner_row; i < MT9V03X_H - 2; i++)
            {
                Left_Line[i] = (i - intercept_l) / slope_l_rate;//y = kx+b
                Left_Line[i] = (uint8)limit_a_b((int16_t)Left_Line[i], border_min, border_max);//限幅
            }

            //计算斜率
            Get_K_b(Right_Line[R_U_corner_row], R_U_corner_row, Right_Line[R_D_corner_row], R_D_corner_row, &slope_l_rate, &intercept_l);
            for (i = R_D_corner_row; i > R_U_corner_row; i--)
            {
                Right_Line[i] = (i - intercept_l) / slope_l_rate;//y = kx+b
                Right_Line[i] = (uint8)limit_a_b((int16_t)Right_Line[i], border_min, border_max);//限幅
            }
        }
        else  if((L_U_corner_flag) && (!R_U_corner_flag) && (L_D_corner_flag)&& (R_D_corner_flag))
        {

            Get_K_b(Left_Line[L_D_corner_row], L_D_corner_row, Left_Line[L_U_corner_row],L_U_corner_row,&slope_l_rate,&intercept_l);
            for (i = L_D_corner_row; i > L_U_corner_row; i--)
            {
                Left_Line[i] = (i - intercept_l)/ slope_l_rate;//y = kx+b
                Left_Line[i] = (uint8)limit_a_b((int16_t)Left_Line[i], border_min, border_max);//限幅
            }

            //计算斜率
            start = R_D_corner_row + 15;//起点
            start = limit_a_b(start,5, MT9V03X_H - 5);//限幅
            end = R_D_corner_row + 2;//终点
            end = limit_a_b(end, 5, MT9V03X_H - 5);
            Get_K_b(Right_Line[start], start, Right_Line[end], end, &slope_l_rate, &intercept_l);
            for (i = 5; i < R_D_corner_row+5; i++)
            {
                Right_Line[i] = (i - intercept_l) / slope_l_rate;//y = kx+b
                Right_Line[i] = (uint8)limit_a_b((int16_t)Right_Line[i], border_min, border_max);
            }
        }
        else if((!L_U_corner_flag) && (R_U_corner_flag) && (L_D_corner_flag)&& (R_D_corner_flag))
        {

            start = L_D_corner_row + 15;//起点
            start = limit_a_b(start,5, MT9V03X_H - 5);//限幅
            end = L_D_corner_row + 2;//终点
            end = limit_a_b(end, 5, MT9V03X_H - 5);
            Get_K_b(Left_Line[start], start, Left_Line[end], end, &slope_l_rate, &intercept_l);
            for (i = 5 ; i < L_D_corner_row+5; i++)
            {
                Left_Line[i] = (i - intercept_l) / slope_l_rate;//y = kx+b
                Left_Line[i] = (uint8)limit_a_b((int16_t)Left_Line[i], border_min, border_max);
            }

            Get_K_b(Right_Line[R_D_corner_row], R_D_corner_row, Right_Line[R_U_corner_row], R_U_corner_row, &slope_l_rate, &intercept_l);
            for (i = R_D_corner_row; i > R_U_corner_row; i--)
            {
                Right_Line[i] = (i - intercept_l) / slope_l_rate;//y = kx+b
                Right_Line[i] =(uint8) limit_a_b((int16_t)Right_Line[i], border_min, border_max);//限幅
            }
        }
        else if ((L_U_corner_flag) && (R_U_corner_flag) && (!L_D_corner_flag)&& (!R_D_corner_flag) /*&& (bin_image[10][MT9V03X_H - 15]) && (bin_image[119][MT9V03X_H - 15])*/)//只有上面两个点 0101
        {
            Image_Flag.Cross_Fill = 2;
            //计算斜率
            start = L_U_corner_row - 7;
            start = limit_a_b(start, 5, MT9V03X_H - 5);
            end = L_U_corner_row-2;
            end = limit_a_b(end, 5, MT9V03X_H - 5);
            Get_K_b(Left_Line[start], start, Left_Line[end], end, &slope_l_rate, &intercept_l);

            //  if(slope_l_rate>=-1.44) slope_l_rate=-1.44;
            for (i = L_U_corner_row; i < MT9V03X_H - 2; i++)
            {
                Left_Line[i] = (i - intercept_l) / slope_l_rate;//y = kx+b
                Left_Line[i] = (uint8)limit_a_b((int16_t)Left_Line[i], border_min, border_max);//限幅
                //  ips200_show_float(50,140,slope_l_rate,3,3);
            }

            //计算斜率
            start = R_U_corner_row - 7;//起点
            start = limit_a_b(start, 5, MT9V03X_H - 5);//限幅
            end = R_U_corner_row-2;//终点
            end = limit_a_b(end, 5, MT9V03X_H - 5);
            Get_K_b(Right_Line[start], start, Right_Line[end], end, &slope_l_rate, &intercept_l);
            //if(slope_l_rate<=1.44) slope_l_rate = 1.44;
            for (i = R_U_corner_row; i < MT9V03X_H - 2; i++)
            {
                Right_Line[i] = (i - intercept_l) / slope_l_rate;//y = kx+b
                Right_Line[i] = (uint8)limit_a_b((int16_t)Right_Line[i], border_min, border_max);
            }

            // ips200_show_float(150,140,slope_l_rate,3,3);
            // printf("找到十字\r\n");
        }
        else
        {
            Image_Flag.Cross_Fill = 0;
        }
    }
    else
    {
        Image_Flag.Cross_Fill = 0;
    }

}
//=======================================================================
int change_count=0;//跳变计数
void Zebra_Stripes_Detect(int start_line,int endl_ine)
{
    int i=0,j=0;
   change_count=0;//跳变计数
   // int narrow_road_count=0;
//    if(Image_Flag.Zerba!=0 || Image_Flag.Right_Circle!=0 || Image_Flag.Left_Circle != 0)//元素互斥
//    {
//        return;
//    }

    ////赛宽变化判斑马线
//    if(hightest<=60&&
//            20<=Longest_White_Column_Left[1]&&Longest_White_Column_Left[1]<=MT9V03X_W-20&&
//            20<=Longest_White_Column_Right[1]&&Longest_White_Column_Right[1]<=MT9V03X_W-20)
//    {
//        //截止行长，.最长白列的位置在中心附近，边界起始点靠下
//        for(i=65; i>=20; i--) //在靠下的区域进行寻找赛道宽度过窄的地方
//        {
//            if((Standard_Road_Wide[i]-Road_Wide[i])>10)
//            {
//                narrow_road_count++;//多组赛宽变窄，才认为是斑马线
//                if(narrow_road_count>=5)
//                {
//                    start_line=i;//记录赛道宽度很窄的位置
//                    break;
//                }
//            }
//        }
//    }

//    if(start_line!=0)//多组赛宽变窄，，以赛道过窄的位置为中心，划定一个范围，进行跳变计数
//    {
//        start_line=start_line+8;
//        endl_ine=start_line-15;
    if(start_line>=MT9V03X_H-1)//限幅保护，防止数组越界
    {
        start_line=MT9V03X_H-1;
    }
    if(endl_ine<=0)//限幅保护，防止数组越界
    {
        endl_ine=0;
    }

    for(i=start_line; i>=endl_ine; i--) //区域内跳变计数
    {
        for(j=l_border[i]; j<=r_border[i]; j++)
        {
            if(bin_image[i][j+1]-bin_image[i][j]!=0)
            {
                change_count++;
            }
        }
    }
  //  ips200_show_uint(3,166,change_count,7);//debug使用，查看跳变数，便于适应赛道
//    }
//        Draw_Line( Left_Line[start_line], start_line, Left_Line[endl_ine], endl_ine);//画出区域，便于找bug，debug使用
//        Draw_Line( Left_Line[start_line], start_line, Right_Line[start_line], start_line);
//        Draw_Line(Right_Line[endl_ine], endl_ine, Right_Line[start_line], start_line);
//        Draw_Line(Right_Line[endl_ine], endl_ine, Left_Line[endl_ine], endl_ine);
//        ips200_draw_line ( Left_Line[start_line], start_line, Left_Line[endl_ine], endl_ine, RGB565_RED);
//        ips200_draw_line ( Left_Line[start_line], start_line, Right_Line[start_line], start_line, RGB565_RED);
//        ips200_draw_line (Right_Line[endl_ine], endl_ine, Right_Line[start_line], start_line, RGB565_RED);
//        ips200_draw_line (Right_Line[endl_ine], endl_ine, Left_Line[endl_ine], endl_ine, RGB565_RED);

    if(change_count>300)//跳变大于某一阈值，认为找到了斑马线//300
    {
        Image_Flag.Zerba=1;
    //    Set_Beeptime(1);
    }
    else
    {
        Image_Flag.Zerba=0;
    }
}

//==============================================================================================
void ball_in(void);

uint8 a;
void image_process(void)
{
  uint16 i;
        uint8 hightest = 0;                 //定义一个最高行，tip：这里的最高指的是y值的最小

        Threshold=otsuThreshold(base_image[0],MT9V03X_W, MT9V03X_H);
        ips200_show_int(0,138,Threshold,3);
        turn_to_bin();
        image_filter(bin_image);
        image_draw_rectan(bin_image);               //画方
        data_stastics_l = 0;
        data_stastics_r = 0;
        if(get_start_point(image_h - 2))
        {
           // printf("正在开始八领域\n");
            search_l_r((uint16)USE_num,bin_image,&data_stastics_l, &data_stastics_r,start_point_l[0],
                            start_point_l[1], start_point_r[0], start_point_r[1],&hightest);
           // printf("八邻域已结束\n");
            get_left(data_stastics_l);
            get_right(data_stastics_r);

            //图像处理
            a=get_lost_line(l_border,r_border);
            ips200_show_int(0,200,a,1);
            ips200_show_int(50,200,Image_Flag.Cross_Fill,1);

            get_turning_point(l_border,r_border,image_h);
            Zebra_Stripes_Detect(115,40);
            ips200_show_int(170,300,change_count,5);

            if(part==0||part==1)
            {
                if(L_U_corner_flag==1&&L_D_corner_flag==1&&part==0&&a==2)
                   {
                        cut_flag=1;
                        part=1;
                   }
            }
            else if(part==2)
            {
                out=1;
                if(L_U_corner_flag==1)
                {
                  int startX = L_U_corner_col;
                               int startY = L_U_corner_row;
                               int endX = image_w - 1;
                               int endY = image_h - 5;

                               for (int i = 0; i < image_h; i++)
                               {
                                   if (endY != startY)
                                   {
                                       int line_x = ((i - startY) * (endX - startX)) / (endY - startY) + startX;
                                       line_x = limit_a_b(line_x, 0, image_w - 1);

                                       if (i >= L_U_corner_row && i <= endY)
                                       {
                                           if (line_x > l_border[i])
                                           {
                                               r_border[i] = (uint8)line_x;
                                           }
                                       }
                                   }
                               }
                }
                else
                {
                    int startX = 0;
                                                  int startY = 0;
                                                  int endX = image_w - 1;
                                                  int endY = image_h - 5;

                                                  for (int i = 0; i < image_h; i++)
                                                  {
                                                      if (endY != startY)
                                                      {
                                                          int line_x = ((i - startY) * (endX - startX)) / (endY - startY) + startX;
                                                          line_x = limit_a_b(line_x, 0, image_w - 1);

                                                          if (i >= L_U_corner_row && i <= endY)
                                                          {
                                                              if (line_x > l_border[i])
                                                              {
                                                                  r_border[i] = (uint8)line_x;
                                                              }
                                                          }
                                                      }
                                                  }
                }



            }
            else
            {
                 Cross_fill(l_border,r_border);
            }
        }

  // ====================================================================
        // 计算原始中线
            for (i = hightest; i < image_h-1; i++)
            {
                center_line[i] = (l_border[i] + r_border[i]) >> 1;
            }

            // 将八邻域中线复制到处理管线
               for (i = 0; i < image_h; i++)
               {
                   Mid_Line[i] = center_line[i];  // 复制到Mid_Line供后续处理
               }

             // 执行中线拟合和滤波
                   fit_midline();    // 中线拟合
                   HDPJ_lvbo();      // 滑动平均滤波

                   // 显示最终的中线（使用处理后的End_Mid_Line）
//                       for (i = 3; i < image_h-3; i++)
//                       {
//                           ips200_draw_point(End_Mid_Line[i], i, RGB565_RED);  // 用黄色显示处理后的中线// 在 image_process()
//                           ips200_draw_point(l_border[i]+1, i, RGB565_BLUE);  // 用黄色显示处理后的中线// 在 image_process()
//                           ips200_draw_point(l_border[i]+2, i, RGB565_RED);  // 用黄色显示处理后的中线// 在 image_process()
//                           ips200_draw_point(r_border[i]-1, i, RGB565_GREEN);  // 用黄色显示处理后的中线// 在 image_process()
//                           ips200_draw_point(r_border[i]-2, i, RGB565_RED);  // 用黄色显示处理后的中线// 在 image_process()
//                       }

                       // 检查并显示左上拐点坐标 (x, y)
//                                   if(L_U_corner_flag)
//                                   {
//                                       ips200_show_int(0, 270, L_U.._corner_col, 3); // 在 (0, 160) 位置显示 X 坐标
//                                       ips200_show_int(40, 270, L_U_corner_row, 3); // 在 (40, 160) 位置显示 Y 坐标
//                                   }
//
//                                   // 检查并显示右上拐点坐标 (x, y)
//                                   if(R_U_corner_flag)
//                                   {
//                                       ips200_show_int(100, 270, R_U_corner_col, 3); // 在 (100, 160) 位置显示 X 坐标
//                                       ips200_show_int(140, 270,R_U_corner_row, 3); // 在 (140, 160) 位置显示 Y 坐标
//                                   }
//
//
//                                   // 检查并显示左下拐点坐标 (x, y)
//                                   if(L_D_corner_flag)
//                                   {
//                                       ips200_show_int(0, 300, L_D_corner_col, 3); // 在 (0, 180) 位置显示 X 坐标
//                                       ips200_show_int(40, 300, L_D_corner_row, 3); // 在 (40, 180) 位置显示 Y 坐标
//                                   }
//
//                                   // 检查并显示右下拐点坐标 (x, y)
//                                   if(R_D_corner_flag)
//                                   {
//                                       ips200_show_int(100, 300, R_D_corner_col, 3); // 在 (100, 180) 位置显示 X 坐标
//                                       ips200_show_int(140, 300, R_D_corner_row, 3); // 在 (140, 180) 位置显示 Y 坐标
//                                   }
}



void ball_in(void)
{
    Draw_Line(L_U_corner_col,L_U_corner_row,image_w-1,80);
}


////=======================================================一个函数，处理判断是否丢线
//uint8 is_lost_left_line(uint8 *left_line,uint16 height)
//{
//
//    uint16_t lost_point_count=0;
//    uint16 check_threshold = 7; // 假设连续5个点为边界值就认为丢失
//    for(uint16 i=height-2;i>0;i--)
//    {
//        if(left_line[i]<=border_min+3)
//        {
//            lost_point_count++;
//        }
//        else
//        {
//            lost_point_count=0;
//        }
//        if(lost_point_count>=check_threshold)
//        {
//            return 1;
//        }
//    }
//
//    return 0;
//}
//
//uint8 is_lost_right_line(uint8 *right_line,uint16 height)
//{
//
//    uint16_t lost_point_count=0;
//    uint16 check_threshold = 7; // 假设连续5个点为边界值就认为丢失
//    for(uint16 i=height-2;i>0;i--)
//    {
//        if(right_line[i]>=border_max-2)
//        {
//            lost_point_count++;
//        }
//        else
//        {
//            lost_point_count=0;
//        }
//        if(lost_point_count>=check_threshold)
//        {
//            return 1;
//        }
//    }
//
//    return 0;
//}

//
////==========================================================
//
//uint8_t L_U_corner_flag = 0;
//uint8_t L_U_corner_row = 0;
//uint8_t L_U_corner_col = 0;
//float L_U_corner_angle = 0.0f;
//
//uint8_t L_D_corner_flag = 0;
//uint8_t L_D_corner_row = 0;
//uint8_t L_D_corner_col = 0;
//float L_D_corner_angle = 0.0f;
//
//uint8_t R_U_corner_flag = 0;
//uint8_t R_U_corner_row = 0;
//uint8_t R_U_corner_col = 0;
//float R_U_corner_angle = 0.0f;
//
//uint8_t R_D_corner_flag = 0;
//uint8_t R_D_corner_row = 0;
//uint8_t R_D_corner_col = 0;
//float R_D_corner_angle = 0.0f;
//
//
//
//
////=========================================================
//void get_down_turning_point(uint16 left_time,uint16 right_time);
//void get_up_turning_point(uint16 left_time,uint16 right_time);
//void Draw_Line(int startX, int startY, int endX, int endY);
//
//
//
///*---------------------------------------------------------------
//【函    数】get_down_turning_point
//【功    能】拐点检测//下拐点 (十字路口、直角弯等)
//【参    数】left_time, right_time
//【返 回 值】无
//【注意事项】已修正左下、右下拐点的几何判断BUG
//----------------------------------------------------------------*/
//void get_down_turning_point(uint16 left_time, uint16 right_time)
//{
//    // --- 初始化左下拐点标志 ---
//    L_D_corner_flag = 0;
//    L_D_corner_row = 0;
//    L_D_corner_col = 0;
//    L_D_corner_angle = 0;
//
//    // --- 左下拐点检测 (L_D_corner) ---
//    if(enable_L_D_corner)
//    {
//        if(data_stastics_l > 13 && start_point_l[1] >= MT9V03X_H / 2 && left_time <= MT9V03X_H / 2 + 20)
//        {
//            for(int i = 0; i < data_stastics_l - 13; i++)
//            {
//                if(points_l[i+12][1] > 5)
//                {
//                    // 1. 角度判断：使用向量点乘判断是否为锐角或直角
//                    if((points_l[i][0] - points_l[i+6][0]) * (points_l[i+12][0] - points_l[i+6][0]) +
//                       (points_l[i][1] - points_l[i+6][1]) * (points_l[i+12][1] - points_l[i+6][1]) >= 0)
//                    {
//                        // 2. 精细几何形态判断：一个开口朝左上的'V'字形
//                        if(points_l[i+6][0] > points_l[i+12][0] &&   // 中间点(P2)在结束点(P3)的右边
//                           points_l[i+6][1] < points_l[i][1] &&       // 中间点(P2)在起始点(P1)的上方 (Y值小)
//                           ((points_l[i+12][0] - points_l[i+6][0]) + (points_l[i][0] - points_l[i+6][0])) <= 0 && // 向量和的X分量指向左
//                           ((points_l[i+12][1] - points_l[i+6][1]) + (points_l[i][1] - points_l[i+6][1])) >= 0)   // 向量和的Y分量指向上
//                        {
//                            L_D_corner_flag = 1;
//                            L_D_corner_row = (uint8_t)points_l[i+6][1];
//                            L_D_corner_col = (uint8_t)points_l[i+6][0];
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//    }
//
//    // --- 初始化右下拐点标志 ---
//    R_D_corner_flag = 0;
//    R_D_corner_row = 0;
//    R_D_corner_col = 0;
//    R_D_corner_angle = 0;
//
//    // --- 右下拐点检测 (R_D_corner) ---
//    if(enable_R_D_corner)
//    {
//        if(data_stastics_r > 13 && start_point_r[1] >= MT9V03X_H / 2 && right_time <= MT9V03X_H / 2 + 20)
//        {
//            for(int i = 0; i < data_stastics_r - 13; i++)
//            {
//                if(points_r[i+12][1] > 5)
//                {
//                    // 1. 角度判断
//                    if((points_r[i][0] - points_r[i+6][0]) * (points_r[i+12][0] - points_r[i+6][0]) +
//                       (points_r[i][1] - points_r[i+6][1]) * (points_r[i+12][1] - points_r[i+6][1]) >= 0)
//                    {
//                        // 2. 精细几何形态判断：一个开口朝右上的'V'字形
//                        if(points_r[i+6][0] < points_r[i+12][0] &&   // 中间点(P2)在结束点(P3)的左边
//                           points_r[i+6][1] < points_r[i][1] &&       // 中间点(P2)在起始点(P1)的上方
//                           ((points_r[i+12][0] - points_r[i+6][0]) + (points_r[i][0] - points_r[i+6][0])) >= 0 && // 向量和的X分量指向右
//                           ((points_r[i+12][1] - points_r[i+6][1]) + (points_r[i][1] - points_r[i+6][1])) >= 0)   // 向量和的Y分量指向上
//                        {
//                            R_D_corner_flag = 1;
//                            R_D_corner_row = (uint8_t)points_r[i+6][1];
//                            R_D_corner_col = (uint8_t)points_r[i+6][0];
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//    }
//}
//
//
///*---------------------------------------------------------------
//【函    数】get_up_turning_point
//【功    能】拐点检测//上拐点 (环岛入口等)
//【参    数】left_time, right_time
//【返 回 值】无
//【注意事项】已修正右上拐点的几何判断BUG
//----------------------------------------------------------------*/
//void get_up_turning_point(uint16 left_time, uint16 right_time)
//{
//    // --- 初始化左上拐点标志 ---
//    L_U_corner_flag = 0;
//    L_U_corner_row = 0;
//    L_U_corner_col = 0;
//    L_U_corner_angle = 0;
//
//    // --- 左上拐点检测 (L_U_corner) ---
//    if(enable_L_U_corner)
//    {
//        if(data_stastics_l > 13 && start_point_l[1] >= MT9V03X_H/2 && left_time <= MT9V03X_H/2 + 20)
//        {
//            for(int i = 7; i < data_stastics_l - 13; i++)
//            {
//                if(points_l[i+12][1] > 5)
//                {
//                    // 1. 角度判断：是否为钝角或直角
//                    if((points_l[i+6][0] - points_l[i][0]) * (points_l[i+12][0] - points_l[i+6][0]) +
//                       (points_l[i+6][1] - points_l[i][1]) * (points_l[i+12][1] - points_l[i+6][1]) >= 0)
//                    {
//                        // 2. 几何形态判断: 一个开口朝右下的倒'V'字形
//                        if(points_l[i+6][0] > points_l[i][0] &&                               // P2在P1右边
//                           points_l[i+6][1] - points_l[i+12][1] > 4 &&                       // P2比P3明显靠下(Y大)，说明急剧向上拐
//                           points_l[i][1] - points_l[i+6][1] < 4 &&                          // P1与P2高度接近
//                           ((points_l[i+12][0] - points_l[i+6][0]) + (points_l[i+6][0] - points_l[i][0])) >= 0 && // 向量和X指向右
//                           ((points_l[i+12][1] - points_l[i+6][1]) + (points_l[i+6][1] - points_l[i][1])) <= 0)   // 向量和Y指向下
//                        {
//                            L_U_corner_flag = 1;
//                            L_U_corner_row = (uint8_t)points_l[i+6][1];
//                            L_U_corner_col = (uint8_t)points_l[i+6][0];
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//    }
//
//    // --- 初始化右上拐点标志 ---
//    R_U_corner_flag = 0;
//    R_U_corner_row = 0;
//    R_U_corner_col = 0;
//    R_U_corner_angle = 0;
//
//    // --- 右上拐点检测 (R_U_corner) ---
//    if(enable_R_U_corner)
//    {
//        if(data_stastics_r > 13 && start_point_r[1] >= MT9V03X_H/2 && right_time <= MT9V03X_H/2 + 20)
//        {
//            for(int i = 7; i < data_stastics_r - 13; i++)
//            {
//                if(points_r[i+12][1] > 5)
//                {
//                    // 1. 角度判断
//                    if((points_r[i+6][0] - points_r[i][0]) * (points_r[i+12][0] - points_r[i+6][0]) +
//                       (points_r[i+6][1] - points_r[i][1]) * (points_r[i+12][1] - points_r[i+6][1]) >= 0)
//                    {
//                        // 2. 几何形态判断: 一个开口朝左下的倒'V'字形
//                        if(points_r[i+6][0] < points_r[i][0] &&                               // P2在P1左边
//                           points_r[i+6][1] - points_r[i+12][1] > 4 &&                       // P2比P3明显靠下(Y大)，说明急剧向上拐
//                           points_r[i][1] - points_r[i+6][1] < 4 &&                          // P1与P2高度接近
//                           ((points_r[i+12][0] - points_r[i+6][0]) + (points_r[i+6][0] - points_r[i][0])) <= 0 && // 向量和X指向左
//                           ((points_r[i+12][1] - points_r[i+6][1]) + (points_r[i+6][1] - points_r[i][1])) <= 0)   // 向量和Y指向下
//                        {
//                            R_U_corner_flag = 1;
//                            R_U_corner_row = (uint8_t)points_r[i+6][1];
//                            R_U_corner_col = (uint8_t)points_r[i+6][0];
//                            break;
//                        }
//                    }
//                }
//            }
//        }
//    }
//}
//
//            // ================== 在这里加入丢失计数逻辑 ==================
//if(is_lost_left_line(l_border, image_h))
//{
//    Left_Lost_Time++;
//}
//else
//{
//    Left_Lost_Time = 0;
//}
//
//if(is_lost_right_line(r_border, image_h))
//{
//    Right_Lost_Time++;
//}
//else
//{
//    Right_Lost_Time = 0;
//}
//// ==========================================================
//
//ips200_show_int(0,300,Left_Lost_Time,4);
//ips200_show_int(50,300,Right_Lost_Time,4);
//get_down_turning_point(Left_Lost_Time,Right_Lost_Time);
//get_up_turning_point(Left_Lost_Time,Right_Lost_Time);

//











