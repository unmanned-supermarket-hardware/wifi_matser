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


struct systemState SystemState;
int  printfNUM = 0	//打印的计数


void initSysValue(void)
{	
	memset(SystemState, 0, sizeof(SystemState));
}



int Car1_CorrectState = -1;
double Car1_FDistance = -1;
double Car1_BDistance = -1;
int Car1_moveState = -1;

int Car2_CorrectState = -1;
double Car2_FDistance = -1;
double Car2_BDistance = -1;
int Car2_moveState = -1;


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
函数功能：等待来自服务器的指定消息类型，解析再进行处理
入口参数：goalType：目标等待  类型
返回  值：无
**************************************************************************/
void parseOrderFromS(int goalType)
{
	u8 getMS[300];
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


				orderValue = cJSON_GetObjectItem(root, "businessType");  //  ×?D￡×??é??
				if (!orderValue) {
					printf("get name faild !\n");
					printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					USART3_RX_STA = 0;
					cJSON_Delete(root);
					continue;
				}

				
				businessType = atoi(orderValue.valuestring);
				if (businessType == goalType)  //进行目标消息类型的处理
					{
						if(goalType == 1)
							{
								;
							}

						if (goalType == 3)
							{
								// 解析 位置
							}

						USART3_RX_STA = 0;
						cJSON_Delete(root);
						return ;
					}
				else
					{
						USART3_RX_STA = 0;
						cJSON_Delete(root);
						continue;
					}
				
			}	
			else
			{
				printf("\r\n the ms   error!!!");
				printf("\r\n USART3_RX_BUF:%s!!!",USART3_RX_BUF);
				
			
			}

			USART3_RX_STA = 0;
	
		}

		
		delay_ms(100);
		
		printfNUM++
		if (printfNUM ==10)
			{
				printf("\r\n waiting order from  server!!!");
				printfNUM =0;
			}
		
	}


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

	atk_8266_quit_trans();
	atk_8266_send_cmd("AT+CIPSEND","OK",20);		 //开始透传 	   

	u3_printf("%s",MS);
	
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
函数功能：查询小车 取货单元的状态，并等待回复
入口参数：无
返回  值：无
**************************************************************************/
void askState2other(void )
{

	u16 jsonSize;
	cJSON *root;
	char *strJson;
	u8 strSend[300];



	
	// 查询car1
	memset(strSend, 0, sizeof(strSend));
	strSend[0] = '#';
	strSend[1] = '!';


	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0007");

	strJson  =cJSON_PrintUnformatted(root);；
	cJSON_Delete(root); 
	
	jsonSize = strlen(strJson);

	strSend[2] = jsonSize >> 8;
	strSend[3] = jsonSize;

	strncpy(strSend+4,strJson,jsonSize);
	
	strSend[jsonSize+4] = '*';
	strSend[jsonSize+5] = crc8_calculate(strJson, jsonSize);
	strSend[jsonSize+6] = '&';
	// 需要打开
	usart2_sendString(strSend,7 + jsonSize);
	myfree(strJson);



	// 查询car2
	memset(strSend, 0, sizeof(strSend));
	strSend[0] = '#';
	strSend[1] = '!';

	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0008");

	strJson  =cJSON_PrintUnformatted(root);；
	cJSON_Delete(root); 
	
	jsonSize = strlen(strJson);

	strSend[2] = jsonSize >> 8;
	strSend[3] = jsonSize;

	strncpy(strSend+4,strJson,jsonSize);
	
	strSend[jsonSize+4] = '*';
	strSend[jsonSize+5] = crc8_calculate(strJson, jsonSize);
	strSend[jsonSize+6] = '&';
	// 需要打开
	usart5_sendString(strSend,7 + jsonSize);
	myfree(strJson);


	
	// 查询取货单元
	memset(strSend, 0, sizeof(strSend));
	strSend[0] = '#';
	strSend[1] = '!';

	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0013");

	strJson  =cJSON_PrintUnformatted(root);；
	cJSON_Delete(root); 
	
	jsonSize = strlen(strJson);

	strSend[2] = jsonSize >> 8;
	strSend[3] = jsonSize;

	strncpy(strSend+4,strJson,jsonSize);
	
	strSend[jsonSize+4] = '*';
	strSend[jsonSize+5] = crc8_calculate(strJson, jsonSize);
	strSend[jsonSize+6] = '&';
	// 需要打开
	usart4_sendString(strSend,7 + jsonSize);
	myfree(strJson);



	printfNUM = 0;
	// 等待回复
	while(1)
	{
		if((SystemState.car1State>0) && (SystemState.car2State>0) && (SystemState.goodsGetterState>0))
		{
			break;
		}
		
		delay_ms(100);
		
		printfNUM++;
		if (printfNUM == 10)
		{
			printf("\r\n waiting for that other device feedback state!")
		}
	}

	
	printf("\r\n master has gotten other state!!")


}



/**************************************************************************
函数功能:检查系统的 状态，主控需要检查当前的位置是否在复位点
入口参数：无
返回  值：无
**************************************************************************/
void   checkSysState(void)
{	
	int ret = 1; // 状态标志		1:ok  0:error
	char errorDesc[500]; 

	cJSON *root, *data;  // 

	int num = 0;
	int numS = 0;

	char* strSend;
	char send[200];


	
	
	printf("\r\nwaiting for feedbacking state")
	delay_ms(3000);
	
	if (SystemState.car1State != 200)
	{
		ret = 0;
		printf("\r\n the state of Car1 is bad,state:%d!!",SystemState.car1State);
		delay_ms(10);
		strcat(errorDesc,"  car1:");
		strcat(errorDesc,SystemState.car1Error);
	}

	if (SystemState.car2State != 200)
	{
		ret = 0;
		printf("\r\n the state of Car2 is bad,state:%d!!",SystemState.car2State);
		delay_ms(10);
		strcat(errorDesc,"  car2:");
		strcat(errorDesc,SystemState.car2Error);
	}

	if (SystemState.goodsGetterState != 200 )
	{
		ret = 0;
		printf("\r\n the state of goodsGetter is bad,state:%d!!",SystemState.goodsGetterState);
		delay_ms(10);
		strcat(errorDesc,"  goodsGetter:");
		strcat(errorDesc,SystemState.goodsGetter);
	}


	// 未到中间的位置
	// 注意：后面需要调整
	if ( (myabs_double(Car1_FDistance-MIDDLE_SPACE)>0.05) || (myabs_double(Car1_FDistance-MIDDLE_SPACE)>0.05))
	{
		ret = 0;
		printf("\r\n the state of location is bad,Cars are not good space!!");
		delay_ms(10);
		strcat(errorDesc,"MIDDLE_SPACE:");
		strcat(errorDesc,"the state of location is bad,Cars are not good space");
	}






	//  给服务器发状态
	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0001");
	cJSON_AddItemToObject(root, "data", data=cJSON_CreateObject());
	
	
	if(ret ==0 )
	{
		cJSON_AddStringToObject(data,"status", "0");
		cJSON_AddStringToObject(data,"errorDesc", errorDesc);
	}
	else
	{
		cJSON_AddStringToObject(data,"status", "4");
		cJSON_AddStringToObject(data,"errorDesc","ok");
	}

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

	// 加协议头帧
	memset(send, 0, sizeof(send));
	send[0] = '#';
	send[1] = '!';
	strncpy(send+2, strSend, num); 
	send[num+2] = '&';
	send[num+3] = '\n';

	WIFISend(send);

	printf("\r\n state to server ,strSend:%s  LEN:%\d",strSend,strlen(strSend));
	aiwacFree(strSend);


	//若状态有问题，停止在这个函数
	while(1)
	{
		if(ret)
		{
			break;
		}
		
		delay_ms(1000)
		printf("\r\nstate error. errorDesc:%s",errorDesc);

	}


}


/**************************************************************************
函数功能：step1:等待  服务器  查询复位情况并反馈复位情况
入口参数：无
返回  值：无
**************************************************************************/
void waitingSAskState(void)
{
	printf("\r\n enter waitingSAskState");
	
	parseOrderFromS(1);  // 等待 服务端 发起 状态查询
	askState2other();	 // 发起两小车和取货单元的状态查询,并等待反馈
	checkSysState();
}

/**************************************************************************
函数功能：step2:等待  服务器  下发订单坐标并反馈取到货
入口参数：无
返回  值：无
**************************************************************************/
void waitingSSendLocation(void)
{
	printf("\r\n enter waitingSSendLocation");


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



/**************************************************************************
函数功能：解析来自小车1的数据，串口2
入口参数：无
返回  值：无
**************************************************************************/
void PaserCar1_State(void)
{



}


/**************************************************************************
函数功能：解析来自小车2的数据，串口5
入口参数：无
返回  值：无
**************************************************************************/
void PaserCar2_State(void)
{



}

/**************************************************************************
函数功能：解析来自取货单元的数据，串口4
入口参数：无
返回  值：无
**************************************************************************/
void PaserGoodsGetter_State(void)
{



}


/**************************************************************************
函数功能：绝对值函数
入口参数：double
返回  值：unsigned int
**************************************************************************/
double myabs_double(double a)
{ 		   
	  double temp;
		if(a<0)  temp=-a;  
	  else temp=a;
	  return temp;
}


