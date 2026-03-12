#ifndef COMMANDS_H
#define COMMANDS_H

#include "config.h"

void handle_freq_set(char *param);
void handle_freq();
void handle_band_switch(char *param);
void handle_freq_add(char *param);
void handle_freq_sub(char *param);
void handle_unknown_command(char *param);

typedef struct {
    char *command;
    void (*action)(char *param);
} Command;

Command command_table[];

#endif 