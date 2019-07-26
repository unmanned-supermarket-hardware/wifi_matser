#include "AIWAC_Supermarket.h"

#define MASTER_AREA_1 "Area1"
#define MASTER_AREA_2 "Area2"

#define MASTER_ID MASTER_AREA_1



//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
const u8* wifista_ssid="AIWAC";			//路由器SSID号
const u8* wifista_encryption="WPA/WPA2_PSK";	//wpa/wpa2 aes加密方式
const u8* wifista_password="epic2012"; 	//连接密码


//连接端口号:8086,可自行修改为其他端口.
const u8* portnum="8899";	

int numF1 = 0;
int numF2 = 0;

int WIFIInitOK = 0;   // 可以进入中断解析函数标志

/**************************************************************************
函数功能：初始化wifi模块
入口参数：无
返回  值：无
**************************************************************************/
void wifi_Init(void)
{
	atk_8266_test();		//进入ATK_ESP8266
}




/**************************************************************************
函数功能：解析来自wifi模块的数据
入口参数：无
返回  值：businessType  	服务器给主控发 的
**************************************************************************/
int parseOrderFromS(void)
{
	u8 getMS[200];
	cJSON *root, *orderValue;  // 
	u16 rlen = 0;

	int businessType =999;
	
	while(1)
	{
		if(USART3_RX_STA&0X8000)		
		{ 
			rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
			USART3_RX_BUF[rlen]=0;	
			printf("\r\nlen:%d",rlen);

		/*
			if(atk_8266_check_cmd("OK"))
			{
				printf("\r\n form 8266 MS");
				printf("\r\n USART3_RX_BUF:%s!!!",USART3_RX_BUF);
				memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));
				USART3_RX_STA==0;
				continue;
			}

		*/


			if (	(USART3_RX_BUF[0] == '#') 
				&&	(USART3_RX_BUF[1] == '!')
				&&	(USART3_RX_BUF[rlen-1] == '&')
				)
			{
	
				strncpy(getMS, USART3_RX_BUF+2, rlen-3); 
				printf("\r\n getMs:%s",getMS);
				
				root = cJSON_Parse(getMS);
				if (!root) 
				{
					printf("Error before: [%s]\n",cJSON_GetErrorPtr());
					USART3_RX_STA = 0;
					continue;
				}
				
				orderValue = cJSON_GetObjectItem(root, "id");  //  ×?D￡×??é??
				if (!orderValue) {
					printf("get name faild !\n");
					printf("Error before: [%s]\n", cJSON_GetErrorPtr());
	
				}
	
				printf("\r\n id:%s",orderValue->valuestring);
				
	
				cJSON_Delete(root);
			}	
			else
			{
				printf("\r\n the ms   error!!!");
				printf("\r\n USART3_RX_BUF:%s!!!",USART3_RX_BUF);
				
			
			}

			USART3_RX_STA = 0;
	
		}
		delay_ms(20);

	}




	
	return businessType;


}


/**************************************************************************
函数功能：给服务端发主控的角色
入口参数：无
返回  值：无
**************************************************************************/
void sendMasterID2S()
{
	cJSON *root, *data;  // 

	int num = 0;
	int numS = 0;

	char* strSend;
	char send[300];


	//  给服务器发
	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0000");
	cJSON_AddItemToObject(root, "data", data=cJSON_CreateObject());
	cJSON_AddStringToObject(data,"ID", MASTER_ID);

	strSend=cJSON_Print(root); 
	cJSON_Delete(root); 


	// 去掉所有\r\n.安卓端是  一行一行的接收
	num = strlen(strSend);
	for(numS = 0;numS < num  ;numS++)
		{
			if ( (strSend[numS] == '\n') ||  (strSend[numS] == '\r') )
				{

					strSend[numS] = ' ';
				}

		}

	strSend[num] = '\n';

/*
	// 加协议头帧
	memset(send, 0, sizeof(send));
	send[0] = '#';
	send[1] = '!';
	strncpy(send+2, strSend, num+1); 
	*/

	// 加协议头帧
	memset(send, 0, sizeof(send));
	send[0] = '#';
	send[1] = '!';
	strncpy(send+2, strSend, num); 
	send[num+2] = '&';
	send[num+3] = '\n';

	WIFISend(send);

	printf("\r\nstrSend:%s  LEN:%\d",strSend,strlen(strSend));
	aiwacFree(strSend);


}




/**************************************************************************
函数功能：wifi模块数据发送函数，封装了下发送的逻辑
入口参数：无
返回  值：无
**************************************************************************/
void WIFISend(char* MS)
{

	WIFIInitOK = 0;

	atk_8266_quit_trans();
	atk_8266_send_cmd("AT+CIPSEND","OK",20);		 //开始透传 	   

	u3_printf("%s",MS);
	WIFIInitOK =1;
	
}



/**************************************************************************
函数功能：主控取货的逻辑
入口参数：数据指针
返回  值：无
**************************************************************************/
void  AIWAC_MasterGetGoods(void)
{
	while(1)
	{
		waitingSAskState();			// 等待服务端查询状态，并反馈
		waitingSSendLocation();		// 获取位置，取货
		DropGoods();				// 放货
		DropPan();					// 放盘子，并复位
		delay_ms(100);
		//printf("\r\n finish one time !!");
	}


}



/**************************************************************************
函数功能：step1:等待  服务器  查询复位情况并反馈复位情况
入口参数：无
返回  值：无
**************************************************************************/
void waitingSAskState(void)
{
	while(1)
	{
		parseOrderFromS();



	}


}

/**************************************************************************
函数功能：step2:等待  服务器  下发订单坐标并反馈取到货
入口参数：无
返回  值：无
**************************************************************************/
void waitingSSendLocation(void)
{
	


}


/**************************************************************************
函数功能：step3:进入放货逻辑并反馈已经放货
入口参数：无
返回  值：无
**************************************************************************/
void DropGoods(void)
{
	


}



/**************************************************************************
函数功能：step4:进入复位丢盘逻辑并反馈已经复位
入口参数：无
返回  值：无
**************************************************************************/
void DropPan(void)
{
	


}


