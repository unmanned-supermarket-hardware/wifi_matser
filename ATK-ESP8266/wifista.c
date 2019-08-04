#include "common.h"
#include "stdlib.h"
#include "cJSON.h"
#include "AIWAC_Supermarket.h"



/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//ATK-ESP8266 WIFI模块 WIFI STA驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2015/4/3
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
/////////////////////////////////////////////////////////////////////////////////////////////////////////// 




void aiwacMalloc(int size)
{
	mymalloc(SRAMIN,size) ;
}


void aiwacFree(void *ptr)
{
	myfree(SRAMIN,ptr)	;
}
  



int time1 = 0;
void  testTS(u8 *chat)
{	

	cJSON *root, *orderValue;  // 

	int num = 0;
	u8 getMS[500];
	char* strSend;
	u8 * p =chat;
	int numS = 0;
	char send[500];

	
	if ( (chat[0] == '#') &&  (chat[1] == '!') )
		{
			num = strlen(chat);
			strncpy(getMS, p+2, num); 
			printf("\r\n getMs:%s",getMS);





			
			root = cJSON_Parse(getMS);
		
			orderValue = cJSON_GetObjectItem(root, "id");  //  ×?D￡×??é??
			if (!orderValue) {
				printf("get name faild !\n");
				printf("Error before: [%s]\n", cJSON_GetErrorPtr());

			}

			printf("\r\n id:%s",orderValue->valuestring);
			

				cJSON_Delete(root);




				if (time1 == 1)
					{
						return;

					}

				
				//  给服务器发
				root=cJSON_CreateObject();
				
				
				cJSON_AddNumberToObject(root,"from", 3);
				strSend=cJSON_Print(root); 
				cJSON_Delete(root); 

				num = strlen(strSend);
				for(numS = 0;numS < num  ;numS++)
					{
						if ( (strSend[numS] == '\n') ||  (strSend[numS] == '\r') )
							{

								strSend[numS] = ' ';
							}

					}

				  
				
				//strcpy(send, strSend);
				strSend[num] = '\n';
				
				//send[num+1] = '\0';
				u3_printf("%s",strSend);
				printf("\r\nstrSend:%s",strSend);
				aiwacFree(strSend);
				time1 =1;




				

		}
	else
		{
			printf("\r\n the ms  from server  error!!!");
		
		}
	


}

//ATK-ESP8266 WIFI STA测试
//用于测试TCP/UDP连接
//返回值:0,正常
//    其他,错误代码
u8 netpro=0;	//网络模式
u8 atk_8266_wifista_test(void)
{
	u8 ipbuf[16]; 	//IP缓存
	u8 res=0;
	u16 rlen=0;
	u8 *p;
	p=mymalloc(SRAMIN,32);							//申请32字节内存


	//设置WIFI STA模式
	while(atk_8266_send_cmd("AT+CWMODE=1","OK",50))
	{
		printf("\r\n set  WIFI STA  mode faied, try...");
		delay_ms(10);
	}
	atk_8266_send_cmd("AT+RST","OK",20);		//DHCP服务器关闭(仅AP模式有效) 		
	delay_ms(1000); 		//延时3S等待重启成功
	delay_ms(1000);
	delay_ms(1000);
	delay_ms(1000);
	printf("\r\n set  WIFI STA  mode ok");


	//连接目标路由器,并且获得IP
	printf("\r\n start to connect  the  router");
	//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
	while(atk_8266_send_cmd(p,"WIFI GOT IP",300))					
		{
			printf("\r\n wating link to router");
			delay_ms(10);
		}
	printf("\r\n connect the router succussfully ");

	


	delay_ms(500);

	atk_8266_at_response(1);//WIFI模块发过来的数据,及时上传给电脑

	strcpy(ipbuf, AIWAC_IP);//  服务端ip
	while(atk_8266_send_cmd("AT+CIPMUX=0","OK",20))
	{
		printf("\r\n waiting for setting the one link");
		delay_ms(10);

	}  //0：单连接，1：多连接


	printf("\r\nstart built a TCP link with srever ");
	sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标TCP服务器
	while(atk_8266_send_cmd(p,"OK",200))  //  一直连服务端，
	{
		printf("\r\n  link TCP  failed，the server may not work!!!  ");
		delay_ms(10);
	}	
	printf("\r\n link TCP   ok");
	
	while(atk_8266_send_cmd("AT+CIPMODE=1","OK",200));      //传输模式为：透传			
	
	
	printf("\r\n My ip :%s",ipbuf);
			
	myfree(SRAMIN,p);		//释放内存 
	return res;		
} 




























