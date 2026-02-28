#ifndef COMMANDS_H
#define COMMANDS_H

void handle_freq_set(char *param);
void handle_freq();
void handle_band_switch(char *param);
void handle_freq_add(char *param);  // 增加频率的命令处理
void handle_freq_sub(char *param);  // 减少频率的命令处理
void handle_unknown_command(char *param);

/* 命令结构体定义 */
typedef struct {
    char *command;       // 命令名称
    void (*action)(char *param);  // 执行该命令的函数，接收命令的参数
} Command;

Command command_table[];

#endif 