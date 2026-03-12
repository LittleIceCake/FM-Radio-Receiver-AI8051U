#include "config.h"
#include "GPIO.h"
#include "Delay.h"
#include "UI.h"
#include "uart.h"
#include "switch.h"
#include "nvic.h"
#include "I2C.h"
#include "RDA5807FP.h"
#include "Commands.h"
#include "string.h"
#include "RTC.h"

u8 Key_Vel = 0;
u8 Timer_Updata_Value = 0;
u8 code Timer_Updata_Value_P = 200;
bit SI_Flag;

void GPIO_config(){
    GPIO_InitTypeDef GPIO_InitStructure;
    EAXSFR();
    
    P0M1 = 0; P0M0 = 0;
    P1M1 = 0; P1M0 = 0;
    P2M1 = 0; P2M0 = 0;
    P3M1 = 0; P3M0 = 0;
    
    GPIO_InitStructure.Pin  = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.Mode = GPIO_PullUp;
    GPIO_Inilize(GPIO_P3,&GPIO_InitStructure);
    
    GPIO_InitStructure.Pin  = GPIO_Pin_3 | GPIO_Pin_4;
    GPIO_InitStructure.Mode = GPIO_OUT_OD;
    GPIO_Inilize(GPIO_P2, &GPIO_InitStructure);
    
    P0IM0=0x00;
    P0IM1=0x00;
    P0INTE = 0xff;
    
    P1IM0=0x00;
    P1IM1=0x00;
    P1INTE = 0xff;
}
/****************  I2C初始化函数 *****************/
void	I2C_config(void)
{
	I2C_InitTypeDef		I2C_InitStructure;

	I2C_InitStructure.I2C_Mode      = I2C_Mode_Master;	//主从选择   I2C_Mode_Master, I2C_Mode_Slave
	I2C_InitStructure.I2C_Enable    = ENABLE;			//I2C功能使能,   ENABLE, DISABLE
	I2C_InitStructure.I2C_MS_WDTA   = DISABLE;			//主机使能自动发送,  ENABLE, DISABLE
	I2C_InitStructure.I2C_Speed     = 58;				//总线速度=Fosc/2/(Speed*2+4),  
                                                        // 400k, 24M => 13
                                                        // 100k, 24M => 58
	I2C_Init(&I2C_InitStructure);
	NVIC_I2C_Init(I2C_Mode_Master,DISABLE,Priority_0);	//主从模式, I2C_Mode_Master, I2C_Mode_Slave; 中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

	I2C_SW(I2C_P14_P15);					//I2C_P14_P15,I2C_P24_P25,I2C_P33_P32
}

void UART_config(void)
{
	COMx_InitDefine		COMx_InitStructure;					//结构定义

	COMx_InitStructure.UART_Mode      = UART_8bit_BRTx;	//模式, UART_ShiftRight,UART_8bit_BRTx,UART_9bit,UART_9bit_BRTx
	COMx_InitStructure.UART_BRT_Use   = BRT_Timer1;			//选择波特率发生器, BRT_Timer1, BRT_Timer2 (注意: 串口2固定使用BRT_Timer2)
	COMx_InitStructure.UART_BaudRate  = 115200ul;			//波特率, 一般 110 ~ 115200
	COMx_InitStructure.UART_RxEnable  = ENABLE;				//接收允许,   ENABLE或DISABLE
	COMx_InitStructure.BaudRateDouble = DISABLE;			//波特率加倍, ENABLE或DISABLE
	UART_Configuration(UART1, &COMx_InitStructure);		//初始化串口1 UART1,UART2,UART3,UART4
	NVIC_UART1_Init(ENABLE,Priority_1);		//中断使能, ENABLE/DISABLE; 优先级(低到高) Priority_0,Priority_1,Priority_2,Priority_3

	UART1_SW(UART1_SW_P30_P31);		//UART1_SW_P30_P31,UART1_SW_P36_P37,UART1_SW_P16_P17,UART1_SW_P43_P44
}

/* 串口接收指令 */
void GET_UART1_RECV(){
    u16 i;
    char command[50]=0;  // 假设命令最多为 50 字节
    char *param;
     // 解析命令
    sscanf((char *)RX1_Buffer, "%s", command);  // 读取命令部分
    // 查找命令映射表，找到对应的命令并执行
    for( i = 0; command_table[i].command != NULL; i++) {
        if(strcmp(command, command_table[i].command) == 0) {
            // 查找命令并执行对应的操作
            param = (char *)&RX1_Buffer[strlen(command) + 1];  // 获取命令后的参数
            command_table[i].action(param);  // 执行对应的命令操作
            return;
        }
    }
    // 如果没有找到匹配的命令，调用默认的未知命令处理函数
    command_table[5].action((char *)RX1_Buffer);  // 默认执行未知命令处理
} 

static void update_freq_data(float freq) {
    Freq_data[0] = (u16)freq;
    Freq_data[1] = (u16)((freq-(u16)freq)*1000+0.5);
    Freq_UI_Update();
}

static void handle_mode_freq(int direction) {
    float temp1 = direction > 0 ? CHAN_ADD(1) : CHAN_SUB(1);
    update_freq_data(temp1);
}

static void handle_mode_vol(int direction) {
    if(direction > 0) {
        Vol_Add(&Vol);
    } else {
        Vol_Sub(&Vol);
    }
    Vol_UI_Update();
}

static void handle_mode_space(int direction) {
    if(direction > 0) {
        if(++Space > 4)Space = 0;
    } else {
        if(--Space > 250)Space = 4;
    }
    Space_Switch(Space);
    Space_UI_Update();
}

static void handle_mode_band(int direction) {
    if(direction > 0) {
        if(++Band > 5)Band = 0;
    } else {
        if(--Band > 250)Band = 5;
    }
    Band_Switch(Band);
    Band_UI_Update();
    Freq_UI_Update();
}

static void handle_mode_operation(int direction) {
    switch(Mode_Set){
        case 1:
            handle_mode_freq(direction);
        break;
        case 2:
            handle_mode_vol(direction);
        break;
        case 3:
            handle_mode_space(direction);
        break;
        case 4:
            handle_mode_band(direction);
        break;
    }
}

void Key_Event(){
    if(Key_Vel){
        switch(Key_Vel){
            case 1:
                handle_mode_operation(-1);
            break;
            case 2:
                if(++Mode_Set==5)Mode_Set=0;
                SET_UI_Update();
            break;
            case 3:
                handle_mode_operation(1);
            break;
            case 4:
                if(--Channel_Num < 1) Channel_Num = 1;
                Channel_UI_Update(Channel_Num);
            break;
            case 5:
                SET_Freq(Channel_Freq[Channel_Num-1]);
                delay_ms(10);
                update_freq_data(Get_Freq());
            break;
            case 6:
                if(++Channel_Num > Channel_Freq_Lenth) Channel_Num = Channel_Freq_Lenth;
                Channel_UI_Update(Channel_Num);
            break;
        }
        Key_Vel =0;
    }
}

/* 定时器0初始化 */
void Timer0_Init(void)		//1毫秒@24.000MHz
{
	AUXR |= 0x80;			//定时器时钟1T模式
	TMOD &= 0xF0;			//设置定时器模式
	TL0 = 0x40;				//设置定时初始值
	TH0 = 0xA2;				//设置定时初始值
	TF0 = 0;				//清除TF0标志
	TR0 = 1;				//定时器0开始计时
}

/* Main */
void main(){
    
    delay_ms(100);
    /* 引脚初始化 */
    GPIO_config();
    UART_config();
    I2C_config();
    RTC_Config();
    Timer0_Init();
    EA = 1;
    /* 外设初始化 */
    RDA5807_Init();
    SET_Freq(97.2);
    SET_VOL(Vol);
    FM_UI();
    ET0 = 1;//打开定时器中断
    //printf("Hello\n");
    while(1){
     delay_ms(100);
        Key_Event();
        if(COM1.RX_TimeOut > 0 && --COM1.RX_TimeOut == 0)		//超时计数
		{
				if(COM1.RX_Cnt > 0)
				{
					GET_UART1_RECV();
				}
				COM1.RX_Cnt = 0;
		}
        /* 时间设置 */
         if(B1S_Flag){
            B1S_Flag = 0;
             Time_Data[0] = YEAR;
             Time_Data[1] = MONTH;
             Time_Data[2] = DAY;
             Time_Data[3] = HOUR;
             Time_Data[4] = MIN;
             Time_Data[5] = SEC;
             Time_UI_Update();
        }
        if(SI_Flag){
            SI_Flag = 0;
            SI = Get_SI();
            SI_UI_Update();
        }
    }
}

/* 定时器0中断函数 */  //定时1ms 24MHz
void Timer0_Isr(void) interrupt 1
{
    if(++Timer_Updata_Value>Timer_Updata_Value_P){
        Timer_Updata_Value = 0;
        SI_Flag = 1;
    }
    
}


static void check_key_press(unsigned char intf, unsigned char mask, unsigned char pin, u8 key_value) {
    if(intf & mask && pin == 0) {
        delay_ms(20);
        Key_Vel = key_value;
        delay_ms(20);
    }
}

void common_isr()interrupt 13{
    unsigned char intf1;
    unsigned char intf2;
    delay_ms(100);
    intf1=P0INTF;
    if(intf1){
        P0INTF = 0x00;
        check_key_press(intf1, 0x02, P01, 4);
        check_key_press(intf1, 0x04, P02, 5);
        check_key_press(intf1, 0x08, P03, 6);
    }
    intf2=P1INTF;
    if(intf2){
        P1INTF = 0x00;
        check_key_press(intf2, 0x04, P12, 3);
        check_key_press(intf2, 0x08, P13, 2);
        check_key_press(intf2, 0x10, P14, 1);
    }
    if(RTCIF&0x08){
        RTCIF &= ~0x08;
        B1S_Flag =1;
    }
}