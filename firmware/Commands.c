#include "Commands.h"

#include "type_def.h"
#include <stdio.h>
#include <stdlib.h>  // atoi()
#include <string.h>  // strcmp()

/* 这里添加需要调用的头文件 */
#include "RDA5807FP.h"
#include "RTC.h"
#include "UI.h"

extern u16 Freq_data[]; 
extern u8 Band; 

/*      处理不同命令的函数    */
void handle_freq_set(char *param) {
    float freq_value = atof(param);  // 将命令参数转换为整数
    float temp1;
    SET_Freq(freq_value);   // 调用设置频率的函数
    temp1 = Get_Freq();
    Freq_data[0] = (u16)temp1;
    Freq_data[1] = (u16)((temp1-(u16)temp1)*1000+0.5);
    Freq_UI_Update();
    printf("设置频率为: %.2f MHz\n", (float)freq_value);
}

void handle_freq() {
    float temp_freq = Get_Freq();  // 获取当前频率
    printf("当前频率为: %.3f MHz\n", temp_freq);
}

void handle_band_switch(char *param) {
    int band_value = atoi(param);  // 解析命令参数
    Band = band_value;
    Band_Switch(band_value);       // 切换频带
    Band_UI_Update();
    Freq_UI_Update();
    printf("频带切换为: %d\n", band_value);
}

void handle_unknown_command(char *param) {
    printf("未知命令: %s\n", param);
}

void handle_freq_add(char *param) {
    float temp1;
    int freq_value = atoi(param);  // 将命令参数转换为整数
    temp1 = CHAN_ADD(freq_value);  // 调用增加频率的函数
    Freq_data[0] = (u16)temp1;
    Freq_data[1] = (u16)((temp1-(u16)temp1)*1000+0.5);
    Freq_UI_Update();
    printf("频率增加了: %d\n", freq_value);
}

void handle_freq_sub(char *param) {
    float temp1;
    int freq_value = atoi(param);  // 将命令参数转换为整数
    temp1 = CHAN_SUB(freq_value);  // 调用减少频率的函数
    Freq_data[0] = (u16)temp1;
    Freq_data[1] = (u16)((temp1-(u16)temp1)*1000+0.5);
    Freq_UI_Update();
    printf("频率减少了: %d\n", freq_value);
}

/* 辅助函数：判断闰年 */
u8 is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

/* 辅助函数：判断日期合法性 */
u8 is_valid_date(int year, int month, int day) {
    int xdata days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    if (year < 0 || month < 1 || month > 12 || day < 1) {
        return 0;
    }
    // 如果是闰年，调整二月天数
    if (is_leap_year(year)) {
        days_in_month[1] = 29;
    }

    return day <= days_in_month[month - 1];
}

/* 辅助函数：判断时间合法性 */
u8 is_valid_time(int hour, int minute, int second) {
    return (hour >= 0 && hour < 24) && (minute >= 0 && minute < 60) && (second >= 0 && second < 60);
}

/* 时间设定函数 */
void handle_time_set(char *param) {
    int hour, minute, second;
    if (sscanf(param, "%d:%d:%d", &hour, &minute, &second) == 3) {
        if (is_valid_time(hour, minute, second)) {
            // 调用设置时间的函数（假设函数为 SET_Time）
            SET_Time(hour, minute, second);
            printf("设置时间为: %02d:%02d:%02d\n", hour, minute, second);
        } else {
            printf("无效的时间: %s\n", param);
        }
    } else {
        printf("时间格式错误，应为 HH:MM:SS\n");
    }
}

/* 日期设定函数 */
void handle_date_set(char *param) {
    int year, month, day;
    if (sscanf(param, "%d-%d-%d", &year, &month, &day) == 3) {
        if (is_valid_date(year, month, day)) {
            // 调用设置日期的函数（假设函数为 SET_Date）
             SET_Date(year, month, day);
            printf("设置日期为: %04d-%02d-%02d\n", year, month, day);
        } else {
            printf("无效的日期: %s\n", param);
        }
    } else {
        printf("日期格式错误，应为 YYYY-MM-DD\n");
    }
}

/* 日期与时间设定函数 */
void handle_datetime_set(char *param) {
    int year, month, day, hour, minute, second;
    if (sscanf(param, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second) == 6) {
        if (is_valid_date(year, month, day) && is_valid_time(hour, minute, second)) {
            // 调用设置日期和时间的函数（假设函数为 SET_DateTime）
            SET_DateTime(year, month, day, hour, minute, second);
            printf("设置日期和时间为: %04d-%02d-%02d %02d:%02d:%02d\n", year, month, day, hour, minute, second);
        } else {
            printf("无效的日期或时间: %s\n", param);
        }
    } else {
        printf("日期和时间格式错误，应为 YYYY-MM-DD HH:MM:SS\n");
    }
}

/*  命令映射表 */
Command command_table[] = {
    {"Freq_set:", handle_freq_set}, //设置频率
    {"Freq_get", handle_freq},      //获取频率
    {"Band_switch:", handle_band_switch},//频带切换
    {"Freq_add:", handle_freq_add},     // 频率增加
    {"Freq_sub:", handle_freq_sub},     // 频率减少
    {"Time_set:", handle_time_set},       // 时间设定
    {"Date_set:", handle_date_set},       // 日期设定
    {"Datetime_set:", handle_datetime_set}, // 日期与时间设定
    {NULL, handle_unknown_command}  // 默认的未知命令处理
};
