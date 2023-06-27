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

#define usart2_baund  57600//����2�����ʣ�����ָ��ģ�鲨���ʸ���

int main(void)
{	
	u8 key_num;

	u8 key;
	u8 temperature = 0;  	    
	u8 humidity = 0;

	u8 reclen=0;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  	//��ʼ����ʱ����
	uart_init(115200);	//��ʼ������1������Ϊ115200������֧��USMART
	usart2_init(usart2_baund);//��ʼ������2,������ָ��ģ��ͨѶ
	PS_StaGPIO_Init();	//��ʼ��FR��״̬����
	usmart_dev.init(84);//��ʼ��usmart
	BEEP_Init();  			//��ʼ��������
	KEY_Init();					//������ʼ�� 
 	LCD_Init();					//LCD��ʼ��
 	LED_Init();				//��ʼ��LED
	W25QXX_Init();			//��ʼ��W25Q128
	tp_dev.init();			//��ʼ��������
	//usmart_dev.init(168);		//��ʼ��USMART
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 
	exfuns_init();			    //Ϊfatfs��ر��������ڴ�  
 	f_mount(fs[1],"1:",1);  //����FLASH.
	POINT_COLOR=RED;
	
	fingerprint_Init();//��ʼ��ָ��ģ��
	HC05_Init();//��ʼ������ģ��
 	DHT11_Init();//��ʼ����ʪ��ģ��
 	steering_engine_Init();//��ʼ�����ģ��
 	USART3_RX_STA=0;
	
	while(1) {
		
		//ָ��ʶ�𲿷�
		key_num=AS608_get_keynum(0,170);	
		if(key_num) {
			if(key_num==1)Del_FR();		//ɾָ��
			if(key_num==3)Add_FR();		//¼ָ��									
		}
		if(PS_Sta) {	//���PS_Sta״̬���������ָ����
			press_FR();//ˢָ��			
		}

		//�������Ʋ���
		if(USART3_RX_STA&0X8000) {		//���յ�һ������
			reclen=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
	  		USART3_RX_BUF[reclen]=0;	 	//���������
			if(reclen==8||reclen==7) {		//����DS1���
				if(strcmp((const char*)USART3_RX_BUF,"LED1 ON")==0)LED1=0;	//��LED1
				if(strcmp((const char*)USART3_RX_BUF,"LED1 OFF")==0)LED1=1;//�ر�LED1
				BEEP = 1;
				delay_ms(1000);
				BEEP = 0;
			}
			USART3_RX_STA=0;	 
		}
		
		//��ʪ�ȼ�ⲿ��
		key=KEY_Scan(0);
		if(key == KEY0_PRES) {//��KEY0������					  
			DHT11_Read_Data(&temperature,&humidity);		//��ȡ��ʪ��ֵ
			u3_printf("�¶�:%d ʪ��:%d\r\n",temperature,humidity);		//���͵�����ģ��
		}

		//������Ʋ���
		
	} 	
}
