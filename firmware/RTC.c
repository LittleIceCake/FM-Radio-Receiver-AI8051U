#include "RTC.h"

u8 RTC_Data[6] = {24,12,31,23,59,50};
bit B1S_Flag;

void RTC_Config(){
    IRC32KCR =0x80;
    while (!(IRC32KCR &0x01));
    RTCCFG|=0x02;
    INIYEAR=RTC_Data[0];
    INIMONTH=RTC_Data[1];
    INIDAY=RTC_Data[2];
    INIHOUR=RTC_Data[3];
    INIMIN = RTC_Data[4];
    INISEC= RTC_Data[5];
    INISSEC=0;
    RTCIF=0;
    RTCIEN = 0x08;
    RTCCR=0x01;
    RTCCFG|=0x01;
    while(RTCCFG&0x01);
}

void SET_Date(int year, month, day){
    RTC_Data[0] = year-2000;
    RTC_Data[1] = month;
    RTC_Data[2] = day;
    RTC_Config();
}

void SET_Time(int hour, minute, second){
    RTC_Data[3] = hour;
    RTC_Data[4] = minute;
    RTC_Data[5] = second;
    RTC_Config();
}

void SET_DateTime(int year, month, day, hour, minute, second){
    RTC_Data[0] = year-2000;
    RTC_Data[1] = month;
    RTC_Data[2] = day;
    RTC_Data[3] = hour;
    RTC_Data[4] = minute;
    RTC_Data[5] = second;
    RTC_Config();
}

//void RTC_Isr() interrupt 13{
//    if(RTCIF&0x08){
//         RTCIF &= ~0x08;
//        B1S_Flag =1;
//    }
//       
//}