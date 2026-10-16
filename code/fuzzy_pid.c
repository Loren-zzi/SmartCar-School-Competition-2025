

/*
 * fuzzy_pid.c
 *
 *  Created on: 2025年12月13日
 *      Author: zhigu
 */

#include "fuzzy_pid.h"

// 全局变量定义
float err, err_last, kp_m =10, kd_m = 0;
float E = 0.0, EC = 0.0;

turn_pid_t turn_pid = {0.0, 0.0};


/* 模糊PID控制器可以克服传统PID无法实时调整的缺点 */
float Fuzzy_P(float E, float EC)
{

//    /* 输入变量E（误差）的模糊论域 */
//    float EFF[7] = {-70, -40, -20, 0, 20,40 ,70};
//
//    /* 输入变量EC（误差变化率）的模糊论域 */
//    float DFF[7] = {-35, -20, -10, 0, 10, 20, 35};
    /* 输入变量E（误差）的模糊论域 */
    float EFF[7] = {-40, -25, -10, 0, 10,25 ,40};

    /* 输入变量EC（误差变化率）的模糊论域 */
    float DFF[7] = {-20, -12.5, -5, 0, 5,12.5, 20};

    /* 输出变量U（Kp调整量）的模糊论域 */
    float UFF[7];
    uint8_t i2;

    // 初始化输出论域，线性分布
    for(i2 = 0; i2 < 7; i2++)
        UFF[i2] = kp_m / 6 * i2;

    /* 模糊规则表 - 用于调整Kp的规则 */
    // E: NB(负大), NM(负中), NS(负小), ZO(零), PS(正小), PM(正中), PB(正大)
    // EC: 同上
    // 输出: 对应的模糊输出等级
    int rule[7][7] = {
        // EC: 0(NB) 1(NM) 2(NS) 3(ZO) 4(PS) 5(PM) 6(PB)
        {6, 5, 4, 3, 2, 1, 0},  // E = NB
        {5, 4, 3, 2, 1, 0, 1},  // E = NM
        {4, 3, 2, 1, 0, 1, 2},  // E = NS
        {3, 2, 1, 0, 1, 2, 3},  // E = ZO
        {2, 1, 0, 1, 2, 3, 4},  // E = PS
        {1, 0, 1, 2, 3, 4, 5},  // E = PM
        {0, 1, 2, 3, 4, 5, 6},  // E = PB
    };

    float U = 0;  // 去模糊化后的精确输出值

    // 存储隶属度值
    float PF[2] = {0}, DF[2] = {0}, UF[4] = {0};

    // 存储隶属度对应的索引
    int Pn = 0, Dn = 0, Un[4] = {0};

    // 临时变量
    float t1 = 0, t2 = 0, t3 = 0, t4 = 0, temp1 = 0, temp2 = 0;

    /* ************ 输入模糊化过程 ************ */

    /* 1. 对误差E进行模糊化 */
    if(E > EFF[0] && E < EFF[6])  // 在论域范围内
    {
        if(E <= EFF[1])  // 在NB和NM之间
        {
            Pn = -2;  // 对应NB
            PF[0] = (EFF[1] - E) / (EFF[1] - EFF[0]);  // NB的隶属度
        }
        else if(E <= EFF[2])  // 在NM和NS之间
        {
            Pn = -1;  // 对应NM
            PF[0] = (EFF[2] - E) / (EFF[2] - EFF[1]);  // NM的隶属度
        }
        else if(E <= EFF[3])  // 在NS和ZO之间
        {
            Pn = 0;  // 对应NS
            PF[0] = (EFF[3] - E) / (EFF[3] - EFF[2]);  // NS的隶属度
        }
        else if(E <= EFF[4])  // 在ZO和PS之间
        {
            Pn = 1;  // 对应ZO
            PF[0] = (EFF[4] - E) / (EFF[4] - EFF[3]);  // ZO的隶属度
        }
        else if(E <= EFF[5])  // 在PS和PM之间
        {
            Pn = 2;  // 对应PS
            PF[0] = (EFF[5] - E) / (EFF[5] - EFF[4]);  // PS的隶属度
        }
        else if(E <= EFF[6])  // 在PM和PB之间
        {
            Pn = 3;  // 对应PM
            PF[0] = (EFF[6] - E) / (EFF[6] - EFF[5]);  // PM的隶属度
        }
    }
    else if(E <= EFF[0])  // 小于最小边界
    {
        Pn = -2;   // 完全属于NB
        PF[0] = 1; // 隶属度为1
    }
    else if(E >= EFF[6])  // 大于最大边界
    {
        Pn = 3;    // 完全属于PB
        PF[0] = 0; // 隶属度为0（实际上属于PB，但PB是下一档）
        // 注：这里有个小问题，应该是PF[0]=0, PF[1]=1
    }

    PF[1] = 1 - PF[0];  // 计算相邻模糊集的隶属度

    /* 2. 对误差变化率EC进行模糊化 */
    if(EC > DFF[0] && EC < DFF[6])  // 在论域范围内
    {
        if(EC <= DFF[1])  // 在NB和NM之间
        {
            Dn = -2;
            DF[0] = (DFF[1] - EC) / (DFF[1] - DFF[0]);
        }
        else if(EC <= DFF[2])  // 在NM和NS之间
        {
            Dn = -1;
            DF[0] = (DFF[2] - EC) / (DFF[2] - DFF[1]);
        }
        else if(EC <= DFF[3])  // 在NS和ZO之间
        {
            Dn = 0;
            DF[0] = (DFF[3] - EC) / (DFF[3] - DFF[2]);
        }
        else if(EC <= DFF[4])  // 在ZO和PS之间
        {
            Dn = 1;
            DF[0] = (DFF[4] - EC) / (DFF[4] - DFF[3]);
        }
        else if(EC <= DFF[5])  // 在PS和PM之间
        {
            Dn = 2;
            DF[0] = (DFF[5] - EC) / (DFF[5] - DFF[4]);
        }
        else if(EC <= DFF[6])  // 在PM和PB之间
        {
            Dn = 3;
            DF[0] = (DFF[6] - EC) / (DFF[6] - DFF[5]);
        }
    }
    else if(EC <= DFF[0])  // 小于最小边界
    {
        Dn = -2;
        DF[0] = 1;
    }
    else if(EC >= DFF[6])  // 大于最大边界
    {
        Dn = 3;
        DF[0] = 0;
    }

    DF[1] = 1 - DF[0];  // 计算相邻模糊集的隶属度

    /* ************ 模糊推理过程 ************ */

    /* 根据模糊规则表确定输出模糊集的索引 */
    Un[0] = rule[Pn + 2][Dn + 2];  // 矩阵索引需要偏移+2
    Un[1] = rule[Pn + 3][Dn + 2];
    Un[2] = rule[Pn + 2][Dn + 3];
    Un[3] = rule[Pn + 3][Dn + 3];

    /* 使用min算子（取小）计算每条规则的激活强度 */
    if(PF[0] <= DF[0])
        UF[0] = PF[0];
    else
        UF[0] = DF[0];

    if(PF[1] <= DF[0])
        UF[1] = PF[1];
    else
        UF[1] = DF[0];

    if(PF[0] <= DF[1])
        UF[2] = PF[0];
    else
        UF[2] = DF[1];

    if(PF[1] <= DF[1])
        UF[3] = PF[1];
    else
        UF[3] = DF[1];

    /* ************ 处理规则冲突（相同输出） ************ */
    /* 如果多条规则激活同一个输出模糊集，取激活强度最大的 */
    if(Un[0] == Un[1])
    {
        if(UF[0] > UF[1])
            UF[1] = 0;
        else
            UF[0] = 0;
    }
    if(Un[0] == Un[2])
    {
        if(UF[0] > UF[2])
            UF[2] = 0;
        else
            UF[0] = 0;
    }
    if(Un[0] == Un[3])
    {
        if(UF[0] > UF[3])
            UF[3] = 0;
        else
            UF[0] = 0;
    }
    if(Un[1] == Un[2])
    {
        if(UF[1] > UF[2])
            UF[2] = 0;
        else
            UF[1] = 0;
    }
    if(Un[1] == Un[3])
    {
        if(UF[1] > UF[3])
            UF[3] = 0;
        else
            UF[1] = 0;
    }
    if(Un[2] == Un[3])
    {
        if(UF[2] > UF[3])
            UF[3] = 0;
        else
            UF[2] = 0;
    }

    /* ************ 去模糊化过程（重心法） ************ */
    t1 = UF[0] * UFF[Un[0]];
    t2 = UF[1] * UFF[Un[1]];
    t3 = UF[2] * UFF[Un[2]];
    t4 = UF[3] * UFF[Un[3]];
    temp1 = t1 + t2 + t3 + t4;  // 分子：加权和
    temp2 = UF[0] + UF[1] + UF[2] + UF[3];  // 分母：隶属度之和
    U = temp1 / temp2;  // 重心法计算精确输出

    return U;  // 返回Kp调整量
}

/* 类似的模糊控制器，用于调整微分系数Kd */
float Fuzzy_D(float E, float EC)
{
    float EFF[7] = {-40, -25, -10, 0, 10,25 ,40};

    /* 输入变量EC（误差变化率）的模糊论域 */
    float DFF[7] = {-20, -12.5, -5, 0, 5,12.5, 20};
    float UFF[7];
    uint8_t i = 0;

    // 初始化输出论域
    for(i = 0; i < 7; i++)
        UFF[i] = kd_m / 6 * i;

    /* Kd调整的模糊规则表 - 与Kp规则不同 */
    int rule[7][7] = {
        {6, 1, 2, 3, 4, 5, 6},  // E = NB
        {1, 2, 3, 4, 5, 6, 5},  // E = NM
        {2, 3, 4, 5, 6, 5, 4},  // E = NS
        {3, 4, 5, 6, 5, 4, 3},  // E = ZO
        {4, 5, 6, 5, 4, 3, 2},  // E = PS
        {5, 6, 5, 4, 3, 2, 1},  // E = PM
        {6, 5, 4, 3, 2, 1, 0},  // E = PB
    };

    // 后面的实现逻辑与Fuzzy_P类似，只是规则和论域不同
    float U = 0;
    float PF[2] = {0}, DF[2] = {0}, UF[4] = {0};
    int Pn = 0, Dn = 0, Un[4] = {0};
    float t1 = 0, t2 = 0, t3 = 0, t4 = 0, temp1 = 0, temp2 = 0;

    /* 模糊化E */
    if(E > EFF[0] && E < EFF[6])
    {
        if(E <= EFF[1])
        {
            Pn = -2;
            PF[0] = (EFF[1] - E) / (EFF[1] - EFF[0]);
        }
        else if(E <= EFF[2])
        {
            Pn = -1;
            PF[0] = (EFF[2] - E) / (EFF[2] - EFF[1]);
        }
        else if(E <= EFF[3])
        {
            Pn = 0;
            PF[0] = (EFF[3] - E) / (EFF[3] - EFF[2]);
        }
        else if(E <= EFF[4])
        {
            Pn = 1;
            PF[0] = (EFF[4] - E) / (EFF[4] - EFF[3]);
        }
        else if(E <= EFF[5])
        {
            Pn = 2;
            PF[0] = (EFF[5] - E) / (EFF[5] - EFF[4]);
        }
        else if(E <= EFF[6])
        {
            Pn = 3;
            PF[0] = (EFF[6] - E) / (EFF[6] - EFF[5]);
        }
    }
    else if(E <= EFF[0])
    {
        Pn = -2;
        PF[0] = 1;
    }
    else if(E >= EFF[6])
    {
        Pn = 3;
        PF[0] = 0;
    }

    PF[1] = 1 - PF[0];

    /* 模糊化EC */
    if(EC > DFF[0] && EC < DFF[6])
    {
        if(EC <= DFF[1])
        {
            Dn = -2;
            DF[0] = (DFF[1] - EC) / (DFF[1] - DFF[0]);
        }
        else if(EC <= DFF[2])
        {
            Dn = -1;
            DF[0] = (DFF[2] - EC) / (DFF[2] - DFF[1]);
        }
        else if(EC <= DFF[3])
        {
            Dn = 0;
            DF[0] = (DFF[3] - EC) / (DFF[3] - DFF[2]);
        }
        else if(EC <= DFF[4])
        {
            Dn = 1;
            DF[0] = (DFF[4] - EC) / (DFF[4] - DFF[3]);
        }
        else if(EC <= DFF[5])
        {
            Dn = 2;
            DF[0] = (DFF[5] - EC) / (DFF[5] - DFF[4]);
        }
        else if(EC <= DFF[6])
        {
            Dn = 3;
            DF[0] = (DFF[6] - EC) / (DFF[6] - DFF[5]);
        }
    }
    else if(EC <= DFF[0])
    {
        Dn = -2;
        DF[0] = 1;
    }
    else if(EC >= DFF[6])
    {
        Dn = 3;
        DF[0] = 0;
    }

    DF[1] = 1 - DF[0];

    /* 模糊推理 */
    Un[0] = rule[Pn + 2][Dn + 2];
    Un[1] = rule[Pn + 3][Dn + 2];
    Un[2] = rule[Pn + 2][Dn + 3];
    Un[3] = rule[Pn + 3][Dn + 3];

    /* 计算激活强度 */
    if(PF[0] <= DF[0])
        UF[0] = PF[0];
    else
        UF[0] = DF[0];

    if(PF[1] <= DF[0])
        UF[1] = PF[1];
    else
        UF[1] = DF[0];

    if(PF[0] <= DF[1])
        UF[2] = PF[0];
    else
        UF[2] = DF[1];

    if(PF[1] <= DF[1])
        UF[3] = PF[1];
    else
        UF[3] = DF[1];

    /* 处理规则冲突 */
    if(Un[0] == Un[1])
    {
        if(UF[0] > UF[1])
            UF[1] = 0;
        else
            UF[0] = 0;
    }
    if(Un[0] == Un[2])
    {
        if(UF[0] > UF[2])
            UF[2] = 0;
        else
            UF[0] = 0;
    }
    if(Un[0] == Un[3])
    {
        if(UF[0] > UF[3])
            UF[3] = 0;
        else
            UF[0] = 0;
    }
    if(Un[1] == Un[2])
    {
        if(UF[1] > UF[2])
            UF[2] = 0;
        else
            UF[1] = 0;
    }
    if(Un[1] == Un[3])
    {
        if(UF[1] > UF[3])
            UF[3] = 0;
        else
            UF[1] = 0;
    }
    if(Un[2] == Un[3])
    {
        if(UF[2] > UF[3])
            UF[3] = 0;
        else
            UF[2] = 0;
    }

    /* 去模糊化 */
    t1 = UF[0] * UFF[Un[0]];
    t2 = UF[1] * UFF[Un[1]];
    t3 = UF[2] * UFF[Un[2]];
    t4 = UF[3] * UFF[Un[3]];
    temp1 = t1 + t2 + t3 + t4;
    temp2 = UF[0] + UF[1] + UF[2] + UF[3];
    U = temp1 / temp2;

    return U;  // 返回Kd调整量
}

/* 模糊PID控制器主函数 */
void fuzzy_pid_xunji(float *Line_Error)
{
    err = *Line_Error;          // 获取当前误差
    err_last = err;            // 保存为上一次误差
    E = err;                   // 当前误差
    EC = err - err_last;       // 误差变化率

    // 使用模糊逻辑实时调整PID参数
    turn_pid.Kp = Fuzzy_P(E, EC);  // 调整比例系数
    turn_pid.Kd = Fuzzy_D(E, EC);  // 调整微分系数
}
