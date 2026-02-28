#ifndef __RDA5807FP_H
#define __RDA5807FP_H


/* 这里编写I2C的头文件 */
#include "I2C.h"
#include "delay.h"

/* 应用函数 */
void RDA5807_Init();

void SET_Freq(float Freq);//设定频率
void SET_VOL(u8 Vol);//设定音量

float CHAN_ADD(u16 dat);//增加频率
float CHAN_SUB(u16 dat);//减小频率

u8 Get_SI();//获取信号质量
float Get_Freq();//获取频率

u8 Space_Switch(u8 mode);//步进切换
u8 Band_Switch(u8 mode);//频带切换

void Vol_Add(u8* Vol);//增加音量(+10%)
void Vol_Sub(u8* Vol);//减小音量(-10%)
#endif