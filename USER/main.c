#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "lcd.h"
#include "key.h"
#include "beep.h"
#include "touch.h"	  
#include "malloc.h" 
#include "usmart.h"  
#include "sdio_sdcard.h"    
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "usart2.h"
#include "usart3.h"
#include "AS608.h"
#include "timer.h"
#include "fingerprint.h"
#include "hc05.h"
#include "sys.h"
#include "led.h"
#include "string.h"
#include "dht11.h"
#include "stdio.h"
#include "steering_engine.h"

#define usart2_baund  57600//串口2波特率，根据指纹模块波特率更改

int main(void)
{	
	u8 key_num;

	u8 key;
	u8 temperature = 0;  	    
	u8 humidity = 0;

	u8 reclen=0;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  	//初始化延时函数
	uart_init(115200);	//初始化串口1波特率为115200，用于支持USMART
	usart2_init(usart2_baund);//初始化串口2,用于与指纹模块通讯
	PS_StaGPIO_Init();	//初始化FR读状态引脚
	usmart_dev.init(84);//初始化usmart
	BEEP_Init();  			//初始化蜂鸣器
	KEY_Init();					//按键初始化 
 	LCD_Init();					//LCD初始化
 	LED_Init();				//初始化LED
	W25QXX_Init();			//初始化W25Q128
	tp_dev.init();			//初始化触摸屏
	//usmart_dev.init(168);		//初始化USMART
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();			    //为fatfs相关变量申请内存  
 	f_mount(fs[1],"1:",1);  //挂载FLASH.
	POINT_COLOR=RED;
	
	fingerprint_Init();//初始化指纹模块
	HC05_Init();//初始化蓝牙模块
 	DHT11_Init();//初始化温湿度模块
 	steering_engine_Init();//初始化舵机模块
 	USART3_RX_STA=0;
	
	while(1) {
		
		//指纹识别部分
		key_num=AS608_get_keynum(0,170);	
		if(key_num) {
			if(key_num==1)Del_FR();		//删指纹
			if(key_num==3)Add_FR();		//录指纹									
		}
		if(PS_Sta) {	//检测PS_Sta状态，如果有手指按下
			press_FR();//刷指纹			
		}

		//蓝牙控制部分
		if(USART3_RX_STA&0X8000) {		//接收到一次数据
			reclen=USART3_RX_STA&0X7FFF;	//得到数据长度
	  		USART3_RX_BUF[reclen]=0;	 	//加入结束符
			if(reclen==8||reclen==7) {		//控制DS1检测
				if(strcmp((const char*)USART3_RX_BUF,"LED1 ON")==0)LED1=0;	//打开LED1
				if(strcmp((const char*)USART3_RX_BUF,"LED1 OFF")==0)LED1=1;//关闭LED1
				BEEP = 1;
				delay_ms(1000);
				BEEP = 0;
			}
			USART3_RX_STA=0;	 
		}
		
		//温湿度检测部分
		key=KEY_Scan(0);
		if(key == KEY0_PRES) {//当KEY0被按下					  
			DHT11_Read_Data(&temperature,&humidity);		//读取温湿度值
			u3_printf("温度:%d 湿度:%d\r\n",temperature,humidity);		//发送到蓝牙模块
		}

		//舵机控制部分
		
	} 	
}
