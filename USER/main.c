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
	//cJSON *root, *orderValue,*data;  //
	//double length = 0;


	int rlen = 0;
	u8 key,fontok=0; 
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����

	
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
	usart3_init(115200);  	//��ʼ������3������Ϊ115200
	uart2_init(115200); 	// car1
	uart4_init(115200); 	// getter
	uart5_init(115200); 	// car2

	
	//LED_Init();					//��ʼ��LED  
 	//LCD_Init();					//LCD��ʼ��  
 	//KEY_Init();					//������ʼ��  

	
	// W25QXX_Init();				//��ʼ��W25Q128
	// tp_dev.init();				//��ʼ��������

	
	usmart_dev.init(168);		//��ʼ��USMART
	my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ�� 
	my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ�� 
	exfuns_init();				//Ϊfatfs��ر��������ڴ�  
  	//f_mount(fs[0],"0:",1); 		//����SD�� 
 	//f_mount(fs[1],"1:",1); 		//����FLASH.


	// fontok=font_init();		//����ֿ��Ƿ�OK


/*
	printf("\r\n start");
	initSysValue(); 			// ��ʼ��ϵͳ��ȫ�ֱ���
	goStartTogether(FRONT_DIRECTION);	// �ȶ����ٽ��к������
	controlCarToInitSpace();	// �ص���λ��
	goToEverywhere(1, 2, 0.3);
*/

	
/*
	goToLocation(FRONT_DIRECTION, TURING_DISTANCE);
	sendTuringOrder(STATE_TURN_RIGHT);
	
	goToLocation(FRONT_DIRECTION, 0.3);
*/


/*
	root = cJSON_Parse("{\"businessType\":\"0001\"}");
	if (!root) 
	{
		printf("\r\nError before: [%s]\n",cJSON_GetErrorPtr());
		USART3_RX_STA = 0;
		return;
	}


	orderValue = cJSON_GetObjectItem(root, "businessType");  //  ��?D��???��??
	if (!orderValue) {
		printf("get name faild !\n");
		printf("Error before: [%s]\n", cJSON_GetErrorPtr());
		USART3_RX_STA = 0;
		cJSON_Delete(root);
		return;
	}

	
	//businessType = atoi(orderValue->valuestring);
	printf("\r\n businessType:%d",atoi(orderValue->valuestring));

	while(1)
		{
			delay_ms(1000);

		}
*//*
		goToLocation(FRONT_DIRECTION, TURING_DISTANCE);
			sendTuringOrder(STATE_TURN_RIGHT);
			
			goToLocation(FRONT_DIRECTION, 0.4);
*/
/*
	goToLocation(BACK_DIRECTION, TURING_DISTANCE);
	sendTuringOrder(STATE_TURN_LEFT);
	
	goToLocation(BACK_DIRECTION, 2.1);


	while(1)
		{
			printf("\r\n start");
			delay_ms(1000);

		}
*/



	
	// ��֤���ȶ���
	printf("\r\n start");
	
	closeUart2_4_5();
	initSysValue(); 			// ��ʼ��ϵͳ��ȫ�ֱ���

	openUart2_5();
	goStartTogether(FRONT_DIRECTION);	// �ȶ����ٽ��к������
	closeUart2_5();

	
	
	wifi_Init();				// wifiģ���ʼ���������·���������������
	
	sendMasterID2S();			//������˷�������ID

	AIWAC_MasterGetGoods();		// ����ȡ����  ���߼���ʼ
	



/*
	wifi_Init();				// wifiģ���ʼ���������·���������������
	
	sendMasterID2S();			//������˷�������ID

	

	 while(1)
	 	{
	 		parseOrderFromS(3);
			feedbackGotOrder(3);
	 	}
	 
*/
}













