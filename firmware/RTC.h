#ifndef __RTC_H_
#define __RTC_H_

#include "config.h"

extern bit B1S_Flag;

void RTC_Config();
void SET_Date(int year, month, day);
void SET_Time(int hour, minute, second);
void SET_DateTime(int year, month, day, hour, minute, second);


#endif