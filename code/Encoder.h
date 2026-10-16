

#ifndef __ENCODER_H_
#define __ENCODER_H_

#include "zf_common_headfile.h"


struct RC_Para
{
    float temp;  //暂存值,存储RC_baro
    float value; //滤波值
    float RC;    //低通滤波参数
};


extern double speed1,speed2;

typedef struct RC_Para *RC_Filter_pt;
//extern int16 RC_Encoder_L, RC_Encoder_R;  // 低通滤波后

void Encoder_Init(void);
void Encoder_Get(void);


#endif
