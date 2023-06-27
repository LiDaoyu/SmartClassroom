#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"
#include "lcd.h"
#include "usmart.h"		
#include "hc05.h" 	 
#include "usart3.h" 	
#include "key.h" 	 
#include "string.h"	 

//显示ATK-HC05模块的主从状态
void HC05_Role_Show(void)
{
	if(HC05_Get_Role() == 1)
		LCD_ShowString(30,340,200,16,16,"ROLE:Master");				//主机
	else 
		LCD_ShowString(30,340,200,16,16,"ROLE:Slave ");				//从机
}

//显示ATK-HC05模块的连接状态
void HC05_Sta_Show(void)
{												 
	if(HC05_LED)
		LCD_ShowString(120,340,120,16,16,"STA:Connected ");			//连接成功
	else 
		LCD_ShowString(120,340,120,16,16,"STA:Disconnect");	 		//未连接				 
}	 

//ATK-HC05服务函数
int bluetooth_main(void)
{ 
	u8 t;
	u8 key;
	u8 sendmask=0;
	u8 sendcnt=0;
	u8 sendbuf[20];	  
	u8 reclen=0;  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);      //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200
	
	usmart_dev.init(84); 		//初始化USMART		
	LED_Init();					//初始化LED
	KEY_Init();					//初始化按键
 	LCD_Init();		 			//初始化LCD	  
	POINT_COLOR=RED;
	
	delay_ms(1000);			//等待蓝牙模块上电稳定
 	while(HC05_Init()) 		//初始化ATK-HC05模块  
	{
		LCD_ShowString(30,500,200,16,16,"ATK-HC05 Error!"); 
		delay_ms(500);
		LCD_ShowString(30,500,200,16,16,"Please Check!!!"); 
		delay_ms(100);
 	}
	LCD_ShowString(30,500,210,16,16,"KEY_UP:ROLE KEY0:SEND/STOP");  
	LCD_ShowString(30,520,200,16,16,"ATK-HC05 Standby!");  
  	LCD_ShowString(30,560,200,16,16,"Send:");	
	LCD_ShowString(30,580,200,16,16,"Receive:"); 
	POINT_COLOR=BLUE;
	HC05_Role_Show();
	delay_ms(100);
	USART3_RX_STA=0;
 	while(1) 
	{		
		key=KEY_Scan(0);
		if(key==WKUP_PRES)						//切换模块主从设置
		{
   			key=HC05_Get_Role();
			if(key!=0XFF)
			{
				key = !key;  					//状态取反	   
				if(key == 0)
					HC05_Set_Cmd("AT+ROLE=0");
				else 
					HC05_Set_Cmd("AT+ROLE=1");
				HC05_Role_Show();
				HC05_Set_Cmd("AT+RESET");	//复位ATK-HC05模块
				delay_ms(200);
			}
		}else if(key==KEY0_PRES)
		{
			sendmask=!sendmask;				//发送/停止发送  	 
			if(sendmask==0)LCD_Fill(30+40,560,240,160+16,WHITE);//清除显示
		}else delay_ms(10);	   
		if(t==50)
		{
			if(sendmask)					//定时发送
			{
				sprintf((char*)sendbuf,"ALIENTEK HC05 %d\r\n",sendcnt);
	  			LCD_ShowString(30+40,560,200,16,16,sendbuf);	//显示发送数据	
				u3_printf("ALIENTEK HC05 %d\r\n",sendcnt);		//发送到蓝牙模块
				sendcnt++;
				if(sendcnt>99)sendcnt=0;
			}
			HC05_Sta_Show();  	  
			t=0;
			LED0=!LED0; 	     
		}	  
		if(USART3_RX_STA&0X8000)			//接收到一次数据了
		{
			LCD_Fill(30,400,240,320,WHITE);	//清除显示
 			reclen=USART3_RX_STA&0X7FFF;	//得到数据长度
		  	USART3_RX_BUF[reclen]=0;	 	//加入结束符
			if(reclen == 9 || reclen == 8) 		//控制DS1检测
			{
				if(strcmp((const char*)USART3_RX_BUF, "+LED1 ON") == 0)
					LED1=0;	//打开LED1
				if(strcmp((const char*)USART3_RX_BUF, "+LED1 OFF") == 0)
					LED1=1;//关闭LED1
			}
 			LCD_ShowString(30,580,209,119,16,USART3_RX_BUF);//显示接收到的数据
 			USART3_RX_STA=0;	 
		}	 															     				   
		t++;	
	}											    
}


