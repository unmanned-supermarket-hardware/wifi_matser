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
	//u8 netpro=0;	//网络模式
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16]; 	//IP缓存
	u8 *p;
	u16 t=999;		//加速第一次获取链接状态
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//连接状态
	p=mymalloc(SRAMIN,32);							//申请32字节内存
	//atk_8266_send_cmd("AT+CWMODE_DEF=1","OK",50);		//设置WIFI STA模式
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//设置WIFI STA模式

	//设置连接到的WIFI网络名称/加密方式/密码,这几个参数需要根据您自己的路由器设置进行修改!! 
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//设置无线参数:ssid,密码
	while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//连接目标路由器,并且获得IP
PRESTA:



	// 下面是连接成功的逻辑
	netpro = 1;

  //TCP
	
	if(1)     //TCP Client    透传模式测试
	{
		delay_ms(500);

		atk_8266_at_response(1);//WIFI模块发过来的数据,及时上传给电脑

		strcpy(ipbuf, AIWAC_IP);//  服务端ip
		atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0：单连接，1：多连接
		sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //配置目标TCP服务器

		//printf("\r\np:%s len:%d",p,strlen(p));
		while(atk_8266_send_cmd(p,"OK",200))
		{
				printf("\r\n连接TCP失败");
		}	
		
		atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //传输模式为：透传			
	}


	LCD_Clear(WHITE);
	POINT_COLOR=RED;

	// 就为获取 自己Ip展示下
	atk_8266_get_wanip(ipbuf);//服务器模式,获取WAN IP
	sprintf((char*)p,"IP地址:%s 端口:%s",ipbuf,(u8*)portnum);
	Show_Str(30,65,200,12,p,12,0);				//显示IP地址和端口	
	Show_Str(30,80,200,12,"状态:",12,0); 		//连接状态
	Show_Str(120,80,200,12,"模式:",12,0); 		//连接状态
	Show_Str(30,100,200,12,"发送数据:",12,0); 	//发送数据
	Show_Str(30,115,200,12,"接收数据:",12,0);	//接收数据

	// 屏幕展示代码
	atk_8266_wificonf_show(30,180,"请设置路由器无线参数为:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
	POINT_COLOR=BLUE;
	Show_Str(120+30,80,200,12,(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],12,0); 		//连接状态
	USART3_RX_STA=0;


	
	
	/*
	while(1)
	{


		//KEY0 发送数据 
		key=KEY_Scan(0);
		if(key==KEY0_PRES)	
		{
		


			{
				atk_8266_quit_trans();
				atk_8266_send_cmd("AT+CIPSEND","OK",20);         //开始透传           
				sprintf((char*)p,"ATK-8266%d\r\n",t/10);//测试数据
				Show_Str(30+54,100,200,12,p,12,0);

				//  发送数据
				u3_printf("%s",p);
				timex=100;
			}

		}else;

		t++;
		delay_ms(10);


		//接收到一次数据
		if(USART3_RX_STA&0X8000)		
		{ 
			rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
			USART3_RX_BUF[rlen]=0;		//添加结束符 
			printf("get data len:%d,%s",strlen(USART3_RX_BUF),USART3_RX_BUF);	//发送到串口   
			 testTS(USART3_RX_BUF);
			
			sprintf((char*)p,"收到%d字节,内容如下",rlen);//接收到的字节数 
			LCD_Fill(30+54,115,239,130,WHITE);
			POINT_COLOR=BRED;
			Show_Str(30+54,115,156,12,p,12,0); 			//显示接收到的数据长度
			POINT_COLOR=BLUE;
			LCD_Fill(30,130,239,319,WHITE);
			Show_Str(30,130,180,190,USART3_RX_BUF,12,0);//显示接收到的数据  
			USART3_RX_STA=0;
			if(constate!='+')t=1000;		//状态为还未连接,立即更新连接状态
			else t=0;                   //状态为已经连接了,10秒后再检查
		}  



		//连续10秒钟没有收到任何数据,检查连接是不是还存在.
		if(t ==1000)
		{
			LCD_Fill(30+54,125,239,130,WHITE);
			LCD_Fill(60,80,120,92,WHITE);
			constate=atk_8266_consta_check();//得到连接状态
			if(constate=='+')Show_Str(30+30,80,200,12,"连接成功",12,0);  //连接状态
			else Show_Str(30+30,80,200,12,"连接失败",12,0); 	 
			t=0;
		}

		atk_8266_at_response(1);  //  若无数据直接过，有数据即  前面 检测是否断连接的   反馈数据
	}

*/
			
	myfree(SRAMIN,p);		//释放内存 
	return res;		
} 




























