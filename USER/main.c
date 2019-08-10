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
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数

	
	uart_init(115200);		//初始化串口波特率为115200
	usart3_init(115200);  	//初始化串口3波特率为115200
	uart2_init(115200); 	// car1
	uart4_init(115200); 	// getter
	uart5_init(115200); 	// car2

	
	//LED_Init();					//初始化LED  
 	//LCD_Init();					//LCD初始化  
 	//KEY_Init();					//按键初始化  

	
	// W25QXX_Init();				//初始化W25Q128
	// tp_dev.init();				//初始化触摸屏

	
	usmart_dev.init(168);		//初始化USMART
	my_mem_init(SRAMIN);		//初始化内部内存池 
	my_mem_init(SRAMCCM);		//初始化CCM内存池 
	exfuns_init();				//为fatfs相关变量申请内存  
  	//f_mount(fs[0],"0:",1); 		//挂载SD卡 
 	//f_mount(fs[1],"1:",1); 		//挂载FLASH.


	// fontok=font_init();		//检查字库是否OK


/*
	printf("\r\n start");
	initSysValue(); 			// 初始化系统的全局变量
	goStartTogether(FRONT_DIRECTION);	// 先对齐再进行后面操作
	controlCarToInitSpace();	// 回到复位点
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


	orderValue = cJSON_GetObjectItem(root, "businessType");  //  ×?D￡???é??
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



	
	// 保证车先对齐
	printf("\r\n start");
	
	closeUart2_4_5();
	initSysValue(); 			// 初始化系统的全局变量

	openUart2_5();
	goStartTogether(FRONT_DIRECTION);	// 先对齐再进行后面操作
	closeUart2_5();

	
	
	wifi_Init();				// wifi模块初始化，完成连路由器，连服务端逻
	
	sendMasterID2S();			//给服务端发送主控ID

	AIWAC_MasterGetGoods();		// 主控取货的  总逻辑开始
	



/*
	wifi_Init();				// wifi模块初始化，完成连路由器，连服务端逻
	
	sendMasterID2S();			//给服务端发送主控ID

	

	 while(1)
	 	{
	 		parseOrderFromS(3);
			feedbackGotOrder(3);
	 	}
	 
*/
}













