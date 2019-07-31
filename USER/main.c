#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"  
#include "usmart.h"  
#include "sram.h"   
#include "malloc.h" 
#include "w25qxx.h"    
#include "sdio_sdcard.h"
#include "ff.h"  
#include "exfuns.h"    
#include "fontupd.h"
#include "text.h"	
#include "usmart.h"	
#include "touch.h"		
#include "usart3.h"
#include "common.h" 
#include "AIWAC_Supermarket.h"




int main(void)
{        
	int rlen = 0;
	u8 key,fontok=0; 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����

	
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	usart3_init(115200);  //��ʼ������3������Ϊ115200
	uart2_init(115200); 	// car1
	uart4_init(115200); 	// getter
	uart5_init(115200); 	// car2

	
	LED_Init();					//��ʼ��LED  
 	LCD_Init();					//LCD��ʼ��  
 	KEY_Init();					//������ʼ��  

	
	W25QXX_Init();				//��ʼ��W25Q128
	tp_dev.init();				//��ʼ��������
	usmart_dev.init(168);		//��ʼ��USMART
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
  	f_mount(fs[0],"0:",1); 		//����SD�� 
 	f_mount(fs[1],"1:",1); 		//����FLASH.


	fontok=font_init();		//����ֿ��Ƿ�OK


	initSysValue();			// ��ʼ��ϵͳ��ȫ�ֱ���
	
	wifi_Init();				// wifiģ���ʼ���������·���������������
	sendMasterID2S();			//������˷�������ID

	

	AIWAC_MasterGetGoods();		// ����ȡ����  ���߼���ʼ

}













