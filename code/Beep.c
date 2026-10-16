/**
  ******************************************************************************
  * @file    Beep.c
  * @brief   蜂鸣器驱动
    @verbatim

    @endverbatim
  * @{
**/

/* Includes ------------------------------------------------------------------*/
#include "Beep.h"


/* Define\Declare ------------------------------------------------------------*/
#define Beep_Pin P33_1

int16 Beep_Time = 0;
uint8 Beep_Count = 0;
uint16 Beep_Cnt = 0;
/**
 ******************************************************************************
 *  @defgroup 外部调用
 *  @brief
 *
**/

/**@brief   蜂鸣器初始化
-- @param   无
**/
void Beep_Init()
{
    gpio_init(Beep_Pin,GPO,0,GPO_PUSH_PULL);
}

/**@brief   蜂鸣器控制
-- @param   无
**/
void Beep(Beep_Handle Mode)
{
    switch(Mode)
    {
        case On:
            gpio_set_level(Beep_Pin,1);
        break;
        case Off:
            gpio_set_level(Beep_Pin,0);
        break;
    }
}

/**@brief   设置蜂鸣器响固定时间
-- @param   无
**/
void Beep_On()
{
    if(Beep_Time > 0)
    {
        Beep_Time-=1;
        Beep(On);
    }
    else
    {
        Beep(Off);
    }
}

/**@brief   设置蜂鸣器响的时间
-- @param   uint16 Set_Time 时间,单位是ms
**/
void Set_Beeptime(uint16 Set_Time)
{
    if(Beep_Time == 0)
    {
        Beep_Time = Set_Time/5;
    }
}

/**@brief   蜂鸣器判断时间
**/
static bool Bufcnt_Beep(bool Cond,uint16 Cnt)
{
    if(Cond)//满足条件
    {
        if(Beep_Cnt == 0)
        {
            Beep_Cnt = 1;//开始计时
        }
    }

    if(Beep_Cnt >= Cnt)
    {
        Beep_Cnt = 0;
        return true;
    }
    else
    {
        return false;
    }
}

/**@brief   蜂鸣器响固定次数
-- @param   无
**/
void Beep_Freq()
{
    static uint8 Beep_State = 0;
    switch (Beep_State)
    {
        case 0:
            if(Beep_Count > 0)
            {
                Beep_State = 1;
            }
        break;
        case 1:
            Set_Beeptime(100);
            if(Bufcnt_Beep(true,100))
            {
                Beep_State = 2;
                Beep_Count -=1;
            }
        break;
        case 2:
            if(Bufcnt_Beep(true,100))
            {
                Beep_State = 3;
            }
        break;
        case 3:
            if(Beep_Count == 0)
            {
                Beep_State = 0;
            }
            else
            {
                Beep_State = 1;
            }
        break;
    }
}

/**@brief   设置蜂鸣器响的次数
-- @param   uint8 Count 蜂鸣器响的次数  <------------------------------使用直接调用这个   Set_Beepfreq(3);响三下  Set_Beeptime(1000);响一秒
**/
void Set_Beepfreq(uint8 Count)
{
    Beep_Count = Count;
}
