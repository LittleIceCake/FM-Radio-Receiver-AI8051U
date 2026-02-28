#include "RDA5807FP.h"

#include <stdio.h>

/*  在这里替换I2C的API  */
// API格式：(芯片地址，寄存器地址，数据，数据长度)
#define REG_WRITE  I2C_WriteNbyte
#define REG_READ   I2C_ReadNbyte

/* RDA5807FP地址定义 */
#define RDA5807_ADDR    0x11<<1 //连续读写芯片地址为10 间隔读写为11
#define RDA5807_REG2    0X02    //02H寄存器
#define RDA5807_REG3    0X03    //03H寄存器
#define RDA5807_REG4    0X04    //04H寄存器
#define RDA5807_REG5    0X05    //05H寄存器
#define RDA5807_REG6    0X06    //06H寄存器
#define RDA5807_REG7    0X07    //07H寄存器
#define RDA5807_REGA    0x0A    //0AH寄存器
#define RDA5807_REGB    0x0B    //0BH寄存器


//寄存器数据存储
unsigned char xdata buff_RESET_02H[] = {0x00,0x02};//软复位寄存器设置
/* 寄存器设定  */
unsigned char xdata buff_SET_02H[] = {0xD0,0x01};
unsigned char xdata buff_SET_03H[] = {0x00,0x08};
unsigned char xdata buff_SET_04H[] = {0x00,0x00};
unsigned char xdata buff_SET_05H[] = {0x08,0x87};
unsigned char xdata buff_SET_06H[] = {0x00,0x00};
unsigned char xdata buff_SET_07H[] = {0x42,0x02};

/* 模式切换 */
unsigned char xdata band_Set = 2;//频带切换  默认为全球频段 对应03H寄存器 切换后频率置为最小
unsigned char xdata space_Set = 0;//频道间距选择 默认为100Khz 对应03H寄存器 切换不影响当前频率

/* RDA5807FP初始化  */
void RDA5807_Init(){
    
    REG_WRITE(RDA5807_ADDR,RDA5807_REG2,buff_RESET_02H,2);//RDA5807软件复位
    delay_ms(200);
    /*   寄存器初始化      */
    REG_WRITE(RDA5807_ADDR,RDA5807_REG2,buff_SET_02H,2);
    REG_WRITE(RDA5807_ADDR,RDA5807_REG3,buff_SET_03H,2);
    REG_WRITE(RDA5807_ADDR,RDA5807_REG4,buff_SET_04H,2);
    REG_WRITE(RDA5807_ADDR,RDA5807_REG5,buff_SET_05H,2);
    REG_WRITE(RDA5807_ADDR,RDA5807_REG6,buff_SET_06H,2);
    REG_WRITE(RDA5807_ADDR,RDA5807_REG7,buff_SET_07H,2);
}

/* RDA5807FP频率设定 */
//输入参数 Freq  单位：MHZ
void SET_Freq(float Freq){
    u8 band = 0;
    u8 space = 0;
    u16 temp = 0;
    /* 根据频带与间距切换对应参数 */
    switch(band_Set){//频带切换
        case 0:
            band = 87;
        break;
        case 1:
            band = 76;
        break;
        case 2:
            band = 76;
        break;
        case 3:
            band = 65;
        break;
        case 4:
            band = 50;
        break;
    }
    switch(space_Set){//频段切换
        case 0:
            space = 10;
        break;
        case 1:
            space = 5;
        break;
        case 2:
            space = 20;
        break;
        case 3:
            space = 40;
        break;
    }
    //频率控制为03寄存器的高10位
    //根据寄存器所选择的频段与步进设定频率值
    //printf("temp:%f\n",Freq - band);
    //printf("temp:%f\n",(Freq - band)*space);
    //printf("space:%bd\n",space);
    //printf("band:%bd\n",band);
    temp =(u16)((Freq - band)*space+0.5);//   计算CHAN寄存器的值，进行简单的四舍五入，否则在51中频率会少0.1MHz
    
    buff_SET_03H[0] = (u8)(temp>>2);
    buff_SET_03H[1] &= 0x3F;//清空03寄存器低字节的前两位
    buff_SET_03H[1] |= (u8)(temp<<6);
    
    buff_SET_03H[1] |= 0x10;//打开调谐
    REG_WRITE(RDA5807_ADDR,RDA5807_REG3,buff_SET_03H,2);
    //printf("temp:%d\n",temp);
    //printf("3h:%bx,%bx\n",buff_SET_03H[0],buff_SET_03H[1]);
    delay_ms(20);
}

/* RDA5807FP音量设定 */
//输入为：0-100
void SET_VOL(u8 Vol){
    //音量控制为05寄存器的低4位
    u8 temp = Vol*15/100;
    buff_SET_05H[1] &= 0xF0;//清空05寄存器的低四位
    buff_SET_05H[1] |= temp;
    //printf("Temp:%bd\n",temp);
    REG_WRITE(RDA5807_ADDR,RDA5807_REG5,buff_SET_05H,2);
}

/* 频率增加（根据频道间距） */
//此函数调用请注意频带范围
float CHAN_ADD(u16 dat){
    u16 temp = 0;
    temp = (buff_SET_03H[0]<<8)| buff_SET_03H[1];//合并为16位数据
    temp = (temp >> 6) & 0x03FF;//取出前十位
    temp += dat;//增加对应频率
    /* 防止溢出 */
    if (temp > 0x03FF) {
        temp = 0x03FF;  // 如果超出 10 位范围，限制在最大值
    }
    /* 寄存器置位 */
    buff_SET_03H[0] = (u8)(temp>>2);
    buff_SET_03H[1] &= 0x3F;
    buff_SET_03H[1] |= (u8)temp<<6;
    buff_SET_03H[1] |= 0x10;//打开调谐
    REG_WRITE(RDA5807_ADDR,RDA5807_REG3,buff_SET_03H,2);
    
    delay_ms(50);
    return Get_Freq();
}

/* 频率减小（根据频道间距） */
// 此函数调用请注意频带范围
float CHAN_SUB(u16 dat) {
    u16 temp = 0;
    temp = (buff_SET_03H[0]<<8)| buff_SET_03H[1];//合并为16位数据
    temp = (temp >> 6) & 0x03FF;//取出前十位
    /* 防止溢出 */
      if (temp >= dat) {
        temp -= dat;  // 执行减法操作，如果结果大于等于0
    } else {
        temp = 0;  // 防止下溢，确保结果不会小于 0
    }
    /* 寄存器置位 */
    buff_SET_03H[0] = (u8)(temp>>2);
    buff_SET_03H[1] &= 0x3F;
    buff_SET_03H[1] |= (u8)temp<<6;
    buff_SET_03H[1] |= 0x10;//打开调谐
    REG_WRITE(RDA5807_ADDR,RDA5807_REG3,buff_SET_03H,2);
    delay_ms(50);
    return Get_Freq();
}

/*   频段切换    */
//传入参数：0-4
//返回值：成功返回1 失败返回0
u8 Band_Switch(u8 mode){
    if(mode<5){
        switch(mode){
            case 0://87-108MHz
                band_Set = 0;
            break;
            case 1://76-91MHz
                band_Set = 1;
            break;
            case 2://76-108MHz
                band_Set = 2;
            break;
            case 3://65-76MHz
                band_Set = 3;
            break;
            case 4://50-65MHz //此频段还需要修改07寄存器的第九位为0
                band_Set = 4;
            break;
        }
        if(mode<3){
            buff_SET_03H[1] &= 0xF3;     //将对应寄存器清0
            buff_SET_03H[1] |= mode<<2;//将对应寄存器赋值
            
            buff_SET_03H[0] = 0;        //将CHAN寄存器清空
            buff_SET_03H[1] &= 0x3F; 
        }else{
            buff_SET_03H[1] &= 0xF3;     //将对应寄存器清0
            buff_SET_03H[1] |= 3<<2;//将对应寄存器赋值
            
            buff_SET_03H[0] = 0;        //将CHAN寄存器清空
            buff_SET_03H[1] &= 0x3F; 
            
            buff_SET_07H[0] &= 0xFD;
            if(mode == 3){
                buff_SET_07H[0] |= 0x02; //65-76MHz
            }
            REG_WRITE(RDA5807_ADDR,RDA5807_REG7,buff_SET_07H,2);
        }
        REG_WRITE(RDA5807_ADDR,RDA5807_REG3,buff_SET_03H,2);
        return 1;
    }else{
        return 0;
    }   
}

/* 频道间距切换 */
//传入参数：0-3
//返回值：成功返回1 失败返回0
u8 Space_Switch(u8 mode){
    float temp_Freq;
    temp_Freq = Get_Freq();//读取当前频率值
    if(mode<4){
        switch(mode){
            case 0://100KHz
                space_Set = 0;
            break;
            case 1://200KHz
                space_Set = 1;
            break;
            case 2://50Khz
                space_Set = 2;
            break;
            case 3://25Khz
                space_Set = 3;
            break;
        }
        buff_SET_03H[1] &= 0xFC;   //将对应寄存器清0
        buff_SET_03H[1] |= mode;   //将对应寄存器赋值   
        printf("temp_Freq:%f",temp_Freq);
        printf("3h:%bx,%bx\n",buff_SET_03H[0],buff_SET_03H[1]);
        printf("space:%bd\n",space_Set);
        buff_SET_03H[0] = 0;//将CHAN寄存器清空
        buff_SET_03H[1] &= 0x3F;
        printf("03H[1]:%bx",buff_SET_03H[1]);
        SET_Freq(temp_Freq);       //写入当前频率值
        return 1;
        
    }else{
        return 0;
    }
}

/* 检测信号强度 */
//输出范围为0-63
u8 Get_SI(){
    //信号强度保存在0B寄存器中的高六位
    u8 temp = 0;
    REG_READ(RDA5807_ADDR,RDA5807_REGB,&temp,1);
    temp = (temp >> 2) & 0x3F;
    return temp;
}

/*   检测频率  */
//返回单位 MHz
float Get_Freq(){
    //频率保存在0A寄存器中的高六位
    float temp_return = 0;
    u8 band;
    u8 space;
    u8 temp[2] = {0,0};
    switch(band_Set){//频带切换
        case 0:
            band = 87;
        break;
        case 1:
            band = 76;
        break;
        case 2:
            band = 76;
        break;
        case 3:
            band = 65;
        break;
        case 4:
            band = 50;
        break;
    }
    switch(space_Set){//频段切换
        case 0:
            space = 10;
        break;
        case 1:
            space = 5;
        break;
        case 2:
            space = 20;
        break;
        case 3:
            space = 40;
        break;
    }
    REG_READ(RDA5807_ADDR,RDA5807_REGA,temp,2);
    temp_return = (((u16)temp[0] & 0x03) << 8 | temp[1])*1.0/space+band;
    return temp_return;
}

void Vol_Add(u8* Vol){
    if(*Vol<=90){
        *Vol += 10;
        SET_VOL(*Vol);
    }
}

void Vol_Sub(u8* Vol){
    if(*Vol>=10){
        *Vol -= 10;
        SET_VOL(*Vol);
    }
}
