#ifndef __UI_H
#define __UI_H

#include "config.h"

#define Channel_Freq_Lenth  4

extern u8 Time_Data[];
extern u16 Freq_data[];
extern u8 Mode_Set;
extern u8 Vol;
extern u8 Band;
extern u8 Space;
extern u8 SI;
extern u8 Channel_Num;
extern float Channel_Freq[];

void FM_UI();
void Time_UI_Update();
void SET_UI_Update();
void Freq_UI_Update();
void Band_UI_Update();
void Space_UI_Update();
void Vol_UI_Update();
void SI_UI_Update();
void Channel_UI_Update(unsigned char num);

#endif