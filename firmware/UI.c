#include "UI.h"

#include "lcd.h"
#include "lcd_init.h"
#include "pic.h"

u8 Time_Data[6] = {24,12,20,01,01,01}; //时间存储变量 年 月 日 时 分 秒
u16 Freq_data[] = {97,200}; //频率存储变量 MHz KHz
u8 Mode_Set = 0;   //模式设置 
u8 Vol = 80; //音量设置
u8 Band = 2; //频带设置
u8 Space = 0;//步进设置
u8 SI = 0;   //信号获取
float Channel_Freq[Channel_Freq_Lenth] = {97.2,87.8,106.8,99.0};//频道存储值

u8 Channel_Num = 1;

void Time_UI_Update(){
    /* 显示日期 */
    LCD_ShowChar(155,13,'2',WHITE,0x349b,16,0);//显示字符2
    LCD_ShowChar(163,13,'0',WHITE,0x349b,16,0);//显示字符0
    if(Time_Data[0]<10){
        LCD_ShowChar(171,13,'0',WHITE,0x349b,16,0);
        LCD_ShowIntNum(179,13,Time_Data[0],1,WHITE,0x349b,16);//显示年份
    }else{
        LCD_ShowIntNum(171,13,Time_Data[0],2,WHITE,0x349b,16);//显示年份
    }
    LCD_ShowChar(187,13,'-',WHITE,0x349b,16,0);//显示字符
    //补零
    if(Time_Data[1]<10){
        LCD_ShowChar(195,13,'0',WHITE,0x349b,16,0);
        LCD_ShowIntNum(203,13,Time_Data[1],1,WHITE,0x349b,16);//显示月份
    }else{
        LCD_ShowIntNum(195,13,Time_Data[1],2,WHITE,0x349b,16);//显示月份
    }
    LCD_ShowChar(211,13,'-',WHITE,0x349b,16,0);//显示字符
    if(Time_Data[2]<10){
        LCD_ShowChar(219,13,'0',WHITE,0x349b,16,0);
        LCD_ShowIntNum(227,13,Time_Data[2],1,WHITE,0x349b,16);//显示日期
    }else{
        LCD_ShowIntNum(219,13,Time_Data[2],2,WHITE,0x349b,16);//显示日期
    }
    /* 显示时间 */
    
    if(Time_Data[3]<10){
        LCD_ShowChar(245,13,'0',WHITE,0x349b,16,0);//补零
        LCD_ShowIntNum(253,13,Time_Data[3],1,WHITE,0x349b,16);//显示小时
    }else{
        LCD_ShowIntNum(245,13,Time_Data[3],2,WHITE,0x349b,16);//显示小时
    }        
    LCD_ShowChar(261,13,':',WHITE,0x349b,16,0);//显示字符  
    if(Time_Data[4]<10){
        LCD_ShowChar(269,13,'0',WHITE,0x349b,16,0);//补零
        LCD_ShowIntNum(277,13,Time_Data[4],1,WHITE,0x349b,16);//显示分钟
    }else{
        LCD_ShowIntNum(269,13,Time_Data[4],2,WHITE,0x349b,16);//显示分钟
    }        
    LCD_ShowChar(285,13,':',WHITE,0x349b,16,0);//显示字符
    if(Time_Data[5]<10){
        LCD_ShowChar(291,13,'0',WHITE,0x349b,16,0);//补零
        LCD_ShowIntNum(299,13,Time_Data[5],1,WHITE,0x349b,16);//显示秒钟
    }else{
        LCD_ShowIntNum(291,13,Time_Data[5],2,WHITE,0x349b,16);//显示秒钟
    }        
}
void Freq_UI_Update(){
    LCD_ShowIntNum(70,68,Freq_data[0],3,WHITE,0x508c,32);//显示MHz
    LCD_ShowChar(118,68,',',WHITE,0x508c,32,0);//显示字符,
    if(Freq_data[1]>=100){
        LCD_ShowIntNum(134,68,Freq_data[1],3,WHITE,0x508c,32);//显示KHz
    }else if(Freq_data[1]>=10){
        LCD_ShowChar(134,68,'0',WHITE,0x508c,32,0);//显示字符0
        LCD_ShowIntNum(150,68,Freq_data[1],2,WHITE,0x508c,32);//显示KHz
    }else{
        LCD_ShowChar(134,68,'0',WHITE,0x508c,32,0);//显示字符0
        LCD_ShowChar(150,68,'0',WHITE,0x508c,32,0);//显示字符0
        LCD_ShowIntNum(166,68,Freq_data[1],1,WHITE,0x508c,32);//显示KHz
    }
    
    LCD_ShowChar(182,68,',',WHITE,0x508c,32,0);//显示字符,
    LCD_ShowChar(198,68,'0',WHITE,0x508c,32,0);//显示字符0
    LCD_ShowChar(214,68,'0',WHITE,0x508c,32,0);//显示字符0
    LCD_ShowChar(230,68,'0',WHITE,0x508c,32,0);//显示字符0
    
    LCD_ShowChar(266,84,'H',WHITE,0x508c,16,0);//显示字符H
    LCD_ShowChar(274,84,'Z',WHITE,0x508c,16,0);//显示字符Z
}

//更新SET
void SET_UI_Update(){
    LCD_ShowIntNum(87,117,Mode_Set,1,WHITE,0x349b,24);//显示SET模式
}

// 主界面显示函数
void FM_UI(void) {
    LCD_Init();//LCD初始化
    LCD_Fill(0,0,LCD_W,LCD_H,WHITE);
    LCD_ShowPicture(4,2,34,34,LOGO);//绘制黑色小收音机
    LCD_ShowPicture(50,2,90,38,FM_RDA);//英文FM REDIO
    
    /*蓝色圆角矩形-时间背景色*/
    LCD_ShowPicture(150,2,10,38,Blue_Large);
    LCD_ShowPicture(305,2,10,38,Blue_Large);
    LCD_Fill(155,2,310,40,0x349b);
    
    /*紫色圆角矩形-频率背景色*/
    LCD_ShowPicture(10,50,10,50,Purple_Small);//紫色小矩形
    LCD_ShowPicture(305,50,10,50,Purple_Small);//紫色小矩形
    LCD_Fill(15,50,310,100,0x508c);
    
    /*紫色圆角矩形-声音背景色*/
    LCD_ShowPicture(210,110,8,100,Purple_Large);//紫色矩形
    LCD_ShowPicture(306,110,8,100,Purple_Large);//紫色矩形
    LCD_Fill(214,110,311,210,0x508c);
    LCD_ShowString(238,120,"Vol",WHITE,0x508c,32,0);
    LCD_DrawLine(215,155,310,156,WHITE);
    LCD_DrawLine(215,155,216,185,WHITE);
    LCD_DrawLine(215,185,310,186,WHITE);
    LCD_DrawLine(310,155,311,186,WHITE);
    
    /*紫色圆角矩形-信号质量背景色*/
    LCD_ShowPicture(120,110,10,50,Purple_Small);//紫色小矩形
    LCD_ShowPicture(190,110,10,50,Purple_Small);//紫色小矩形
    LCD_Fill(125,110,195,160,0x508c);//覆盖紫色色块
    LCD_ShowString(130,123,"SI:",WHITE,0x508c,24,0);
    
    /*紫色圆角矩形-频道选择背景色*/
    LCD_ShowPicture(120,180,10,50,Purple_Small);//紫色小矩形
    LCD_ShowPicture(190,180,10,50,Purple_Small);//紫色小矩形
    
    LCD_ShowPicture(120,170,10,50,Purple_Small);//紫色小矩形
    LCD_ShowPicture(190,170,10,50,Purple_Small);//紫色小矩形
    LCD_Fill(125,170,195,230,0x508c);//覆盖紫色色块
    LCD_Fill(120,200,200,220,0x508c);//覆盖紫色色块
    LCD_ShowString(130,173,"Chan",WHITE,0x508c,24,0);
    
    /*青色圆角矩形 频道选择界面  */
    LCD_ShowPicture(10,150,10,80,Cyan);//青色矩形
    LCD_ShowPicture(100,150,10,80,Cyan);//青色矩形
    LCD_Fill(15,150,105,230,0x36fa);
    
    /*青色底色前蓝色矩形-序号*/
    LCD_Fill(18,156,38,168,0x349b);
    LCD_Fill(18,174,38,186,0x349b);
    LCD_Fill(18,192,38,204,0x349b);
    LCD_Fill(18,210,38,222,0x349b);
    LCD_ShowString(22,156,"01",WHITE,0x349b,12,0);
    LCD_ShowString(22,174,"02",WHITE,0x349b,12,0);
    LCD_ShowString(22,192,"03",WHITE,0x349b,12,0);
    LCD_ShowString(22,210,"04",WHITE,0x349b,12,0);
    
    /*青色底色前蓝色矩形-频段*/
    LCD_Fill(48,156,100,168,0x349b);
    LCD_Fill(48,174,100,186,0x349b);
    LCD_Fill(48,192,100,204,0x349b);
    LCD_Fill(48,210,100,222,0x349b);
    
    LCD_ShowString(62,156,"FREQ",WHITE,0x349b,12,0);
    LCD_ShowString(62,174,"VOL",WHITE,0x349b,12,0);
    LCD_ShowString(62,192,"SPACE",WHITE,0x349b,12,0);
    LCD_ShowString(62,210,"BAND",WHITE,0x349b,12,0);
    
    /*蓝色圆角矩形-SET*/
    LCD_ShowPicture(10,110,10,38,Blue_Large);
    LCD_ShowPicture(50,110,10,38,Blue_Large);
    LCD_Fill(15,110,55,148,0x349b);
    LCD_ShowString(15,117,"SET",WHITE,0x349b,24,0);
    
    /*蓝色圆角矩形-SET数字*/
    LCD_ShowPicture(65,110,10,38,Blue_Large);
    LCD_ShowPicture(100,110,10,38,Blue_Large);
    LCD_Fill(70,110,105,148,0x349b);
    LCD_ShowChar(75,117,'0',WHITE,0x349b,24,0);//显示字符0
    
    /* 刷新值 */
    SET_UI_Update();
    Time_UI_Update();
    Freq_UI_Update();  
    Band_UI_Update();
    Space_UI_Update();
    Vol_UI_Update();
    SI_UI_Update();
    Channel_UI_Update(1);
}

/* 信号强度界面更新 */
void SI_UI_Update(){
    LCD_ShowIntNum(164,123,SI,2,WHITE,0x508c,24);
}

/* 声音界面更新 */
void Vol_UI_Update(){
    //填充区域 x 218-308 y 157- 183
    LCD_Fill(218,157,Vol/10*9+218,183,WHITE);
    LCD_Fill(Vol/10*9+218,157,308,183,0x508c);
    LCD_ShowIntNum(284,193,Vol,3,WHITE,0x508c,12);
    LCD_ShowString(302,193,"%",WHITE,0x508c,12,0);
}

/* 频道界面更新 */
//输入参数 参数（>=1）
void Channel_UI_Update(u8 num){
    if(num>0){
        char str[20];
        sprintf(str, "%02bd %06.2fMHz",num,Channel_Freq[num-1]);
        LCD_ShowString(125,210,str,WHITE,0x508c,12,0);  
    }
}

/* 步进界面更新 */
void Space_UI_Update(){
    switch(Space){
        case 0:
             LCD_ShowString(250,54,"100KHz",BRRED,0x508c,12,0);
        break;
        case 1:
             LCD_ShowString(250,54,"200KHz",BRRED,0x508c,12,0);
        break;
        case 2:
             LCD_ShowString(250,54," 50KHz",BRRED,0x508c,12,0);
        break;
        case 3:
             LCD_ShowString(250,54," 25KHz",BRRED,0x508c,12,0);
        break;
    }
}

/* 频带界面更新 */
void Band_UI_Update()
{   
    switch(Band){
        case 0:
            LCD_ShowString(40,54,"87,000-108,000",BRRED,0x508c,12,0);
            Freq_data[0] = 87;
            Freq_data[1] = 0;
        break;
        case 1:
            LCD_ShowString(40,54,"76,000-91,000",BRRED,0x508c,12,0);
            Freq_data[0] = 76;
            Freq_data[1] = 0;
        break;
        case 2:
            LCD_ShowString(40,54,"76,000-108,000",BRRED,0x508c,12,0);
            Freq_data[0] = 76;
            Freq_data[1] = 0;
        break;
        case 3:
            LCD_ShowString(40,54,"65,000-76,000",BRRED,0x508c,12,0);
            Freq_data[0] = 65;
            Freq_data[1] = 0;
        break;
        case 4:
            LCD_ShowString(40,54,"50,000-65,000",BRRED,0x508c,12,0);
            Freq_data[0] = 50;
            Freq_data[1] = 0;
        break;
    }
}
