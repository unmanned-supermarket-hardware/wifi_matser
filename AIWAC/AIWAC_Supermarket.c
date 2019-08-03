#include "AIWAC_Supermarket.h"



//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
const u8* wifista_ssid="AIWAC";			//路由器SSID号
const u8* wifista_encryption="WPA/WPA2_PSK";	//wpa/wpa2 aes加密方式
const u8* wifista_password="epic2012"; 	//连接密码

//连接端口号:8086,可自行修改为其他端口.
const u8* portnum="8899";	


struct systemState SystemState;
int  printfNUM = 0	//打印的计数
struct goodsLocation GoodsLocation;

int GotGoodsResult ;		// 取货结果  "Result": int类型, 0 表示成功，-1表示失败
int LoseGoodsResult ;		// 丢货结果  "Result": int类型, 0 表示成功，-1表示失败
int LosePanResult ;			// 丢盘结果  "Result": int类型, 0 表示成功，-1表示失败


int LocationNow = 2;	//A:1  B:2  C:3


void initSysValue(void)
{	
	memset(SystemState, 0, sizeof(SystemState));
	memset(GoodsLocation, 0, sizeof(GoodsLocation));
	
	GotGoodsResult = 666;		// 取货结果  "Result": int类型, 0 表示成功，-1表示失败
	LoseGoodsResult = 666;		// 丢货结果  "Result": int类型, 0 表示成功，-1表示失败
	LosePanResult = 666;		// 丢盘结果  "Result": int类型, 0 表示成功，-1表示失败

	USART2_Car1_jsonParseBuF[0] = '-' ;
	USART4_Getter_jsonParseBuF[0] = '-' ;
	USART5_Car2_jsonParseBuF[0] = '-' ;
}


// 小车的情况
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
	cJSON *root, *orderValue,*data;  // 
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
								printf("\r\n master gets businessTy:%d",businessType);
							}

						if (goalType == 3)
							{
								printf("\r\n master gets businessTy:%d",businessType);
								
								// 解析 位置
								data = cJSON_GetObjectItem(root, "data");
								if (!data) {
									printf("get name faild !\n");
									printf("Error before: [%s]\n", cJSON_GetErrorPtr());
									USART3_RX_STA = 0;
									cJSON_Delete(root);
									continue;
								}

								orderValue = cJSON_GetObjectItem(data, "side");
								if (!orderValue) {
									printf("get name faild !\n");
									printf("Error before: [%s]\n", cJSON_GetErrorPtr());
									USART3_RX_STA = 0;
									cJSON_Delete(root);
									continue;
								}	
								strcpy(GoodsLocation.side,orderValue.valuestring);

								orderValue = cJSON_GetObjectItem(data, "distance");
								if (!orderValue) {
									printf("get name faild !\n");
									printf("Error before: [%s]\n", cJSON_GetErrorPtr());
									USART3_RX_STA = 0;
									cJSON_Delete(root);
									continue;
								}	
								strcpy(GoodsLocation.distance,orderValue.valuestring);


								orderValue = cJSON_GetObjectItem(data, "height");
								if (!orderValue) {
									printf("get name faild !\n");
									printf("Error before: [%s]\n", cJSON_GetErrorPtr());
									USART3_RX_STA = 0;
									cJSON_Delete(root);
									continue;
								}	
								strcpy(GoodsLocation.height,orderValue.valuestring);

								orderValue = cJSON_GetObjectItem(data, "depth");
								if (!orderValue) {
									printf("get name faild !\n");
									printf("Error before: [%s]\n", cJSON_GetErrorPtr());
									USART3_RX_STA = 0;
									cJSON_Delete(root);
									continue;
								}	
								strcpy(GoodsLocation.depth,orderValue.valuestring);
	
							}

						USART3_RX_STA = 0;
						cJSON_Delete(root);
						printf("\r\n master get businessType:%d from server",businessType);
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
	uart5_sendString(strSend,7 + jsonSize);
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
	uart4_sendString(strSend,7 + jsonSize);
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
	feedbackGotOrder(1); //向服务端反馈收到额指令
	askState2other();	 // 发起两小车和取货单元的状态查询,并等待反馈
	checkSysState();	// 检查状态，反馈给服务端
}

/**************************************************************************
函数功能：step2:等待  服务器  下发订单坐标并反馈取到货
入口参数：无
返回  值：无
**************************************************************************/
void waitingSSendLocation(void)
{
	printf("\r\n enter waitingSSendLocation");
	parseOrderFromS(3);  			// 等待 android端向主控提供需要取的货物的位置信息。
	feedbackGotOrder(3); 			// 向服务端反馈收到额指令
	feedbackStartGetGoods(); 		// 通知服务端开始取货
	controlCarToGoodsSpace();		// 控制小车运动到货物点
	notifyGoodsGetterLocation();	// 给取货单元  商品的位置和深度
	waitingGetterGotGoods();		// 等待取货单元反馈取到货
	feedbackGotGoodsResult();		// 反馈已经取到货物
} 


/**************************************************************************
函数功能：step3:进入放货逻辑并反馈已经放货
入口参数：无
返回  值：无
**************************************************************************/
void DropGoods(void)
{
	printf("\r\n enter DropGoods");
	controlCarToGate();				// 控制小车到 放货点
	notifyGoodsGetterLoseGoods();	// 通知取货单元放货
	waitingGetterLoseGoods();		// 等待取货单元放货
	feedbackLoseGoodsResult();		// 给服务端反馈 放货情况
}



/**************************************************************************
函数功能：step4:进入复位丢盘逻辑并反馈已经复位
入口参数：无
返回  值：无
**************************************************************************/
void DropPan(void)
{
	printf("\r\n enter DropPan");

	controlCarToDropPan();		// 控制小车到丢盘子的地方
	notifyGoodsGetterDropPan();	// 通知取货单元丢盘子
	waitingGetterLosePan();	// 等待取货单元丢盘子
	controlCarToInitSpace();	// 控制小车到复位点
	feedbackGoInit();			// 反馈已经复位
}



/**************************************************************************
函数功能：解析来自小车1的数据，串口2
入口参数：无
返回  值：无
**************************************************************************/
void PaserCar1_State(void)
{
	cJSON *root, *orderValue;  // 
	
	
	if (USART2_Car1_jsonParseBuF[0] == '-' ) //  还未收到运动命令
	{
		return;
	}
	
	root = cJSON_Parse(USART2_Car1_jsonParseBuF);
	if (!root) 
		{
			printf("Error before: [%s]\n",cJSON_GetErrorPtr());
		return;
	}

	orderValue = cJSON_GetObjectItem(root, "businessType");  //  businessType
	if (!orderValue) {
			//printf("get name faild !\n");
			//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
			goto end;
		}


	// 反馈小车的状态
	if (strcmp(orderValue.valuestring, "0007")==0)  
		{
			orderValue = cJSON_GetObjectItem(root, "errorCode");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			SystemState.car1State = orderValue.valueint;


			orderValue = cJSON_GetObjectItem(root, "errorDesc");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			strcpy(SystemState.car1Error,orderValue.valuestring );

			goto end;

		}

	// 反馈小车的方向距离
	if (strcmp(orderValue.valuestring, "0011")==0)  
		{
			orderValue = cJSON_GetObjectItem(root, "Co");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car1_CorrectState = orderValue.valuedouble;

			orderValue = cJSON_GetObjectItem(root, "FD");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car1_FDistance = orderValue.valuedouble;

			orderValue = cJSON_GetObjectItem(root, "BD");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car1_BDistance = orderValue.valuedouble;

			orderValue = cJSON_GetObjectItem(root, "mo");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car1_moveState = orderValue.valuedouble;


			goto end;

		}



end :
	cJSON_Delete(root);

}


/**************************************************************************
函数功能：解析来自小车2的数据，串口5
入口参数：无
返回  值：无
**************************************************************************/
void PaserCar2_State(void)
{
	
	cJSON *root, *orderValue;  // 
	
	
	if (USART5_Car2_jsonParseBuF[0] == '-' ) //  还未收到运动命令
	{
		return;
	}
	
	root = cJSON_Parse(USART5_Car2_jsonParseBuF);
	if (!root) 
		{
			printf("Error before: [%s]\n",cJSON_GetErrorPtr());
		return;
	}

	orderValue = cJSON_GetObjectItem(root, "businessType");  //  businessType
	if (!orderValue) {
			//printf("get name faild !\n");
			//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
			goto end;
		}


	// 反馈小车的状态
	if (strcmp(orderValue.valuestring, "0008")==0)  
		{
			orderValue = cJSON_GetObjectItem(root, "errorCode");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			SystemState.car2State = orderValue.valueint;


			orderValue = cJSON_GetObjectItem(root, "errorDesc");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			strcpy(SystemState.car2Error,orderValue.valuestring );

			goto end;

		}

	// 反馈小车的方向距离
	if (strcmp(orderValue.valuestring, "0012")==0)  
		{
			orderValue = cJSON_GetObjectItem(root, "Co");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car2_CorrectState = orderValue.valuedouble;

			orderValue = cJSON_GetObjectItem(root, "FD");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car2_FDistance = orderValue.valuedouble;

			orderValue = cJSON_GetObjectItem(root, "BD");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car2_BDistance = orderValue.valuedouble;

			orderValue = cJSON_GetObjectItem(root, "mo");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car2_moveState = orderValue.valuedouble;


			goto end;

		}



end :
	cJSON_Delete(root);


}

/**************************************************************************
函数功能：解析来自取货单元的数据，串口4
入口参数：无
返回  值：无
**************************************************************************/
void PaserGoodsGetter_State(void)
{
	cJSON *root, *orderValue;  // 
	
	
	if (USART4_Getter_jsonParseBuF[0] == '-' ) //  还未收到运动命令
	{
		return;
	}
	
	root = cJSON_Parse(USART4_Getter_jsonParseBuF);
	if (!root) 
		{
			printf("Error before: [%s]\n",cJSON_GetErrorPtr());
		return;
	}

	orderValue = cJSON_GetObjectItem(root, "businessType");  //  businessType
	if (!orderValue) {
			//printf("get name faild !\n");
			//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
			goto end;
		}



	// 取货单元给主控反馈 状态
	if (strcmp(orderValue.valuestring, "0013")==0)  
		{
			orderValue = cJSON_GetObjectItem(root, "errorCode");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			SystemState.goodsGetterState = orderValue.valueint;

			orderValue = cJSON_GetObjectItem(root, "errorDesc");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			strcpy(SystemState.goodsGetterEorror,orderValue.valuestring );

			goto end;



		}

	// 取货单元给主控反馈 取货情况
	if (strcmp(orderValue.valuestring, "0015")==0)  
		{
			orderValue = cJSON_GetObjectItem(root, "Result");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			GotGoodsResult = orderValue.valueint;

			goto end;

		}

	// 取货单元给主控反馈 卸货情况
	if (strcmp(orderValue.valuestring, "0017")==0)  
		{
			orderValue = cJSON_GetObjectItem(root, "Result");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			LoseGoodsResult = orderValue.valueint;

			goto end;

		}

		// 取货单元给主控反馈 丢盘子情况
	if (strcmp(orderValue.valuestring, "0019")==0)  
		{
			orderValue = cJSON_GetObjectItem(root, "Result");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			LosePanResult = orderValue.valueint;

			goto end;

		}


end :
	cJSON_Delete(root);


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



/**************************************************************************
函数功能：向服务端反馈收到的指令
入口参数：businessTypeGot： 收到的指令
返回  值：无
**************************************************************************/
void feedbackGotOrder(int businessTypeGot)
{
	cJSON *root, *data;  // 

	int num = 0;
	int numS = 0;

	char* strSend;
	char send[200];

	//	给服务器发状态
	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0024");
	cJSON_AddItemToObject(root, "data", data=cJSON_CreateObject());
	cJSON_AddStringToObject(data,"businessTypeGot", stoi(businessTypeGot));



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

	printf("\r\n feedback to server ,strSend:%s  LEN:%\d",strSend,strlen(strSend));
	aiwacFree(strSend);


}


/**************************************************************************
函数功能：向服务端反馈开始取货
入口参数：无
返回  值：无
**************************************************************************/
void feedbackStartGetGoods(void)
{

	cJSON *root, *data;  // 

	int num = 0;
	int numS = 0;

	char* strSend;
	char send[200];

	//	给服务器发状态
	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0004");
	cJSON_AddItemToObject(root, "data", data=cJSON_CreateObject());
	cJSON_AddStringToObject(data,"status", "1");
	cJSON_AddStringToObject(data,"errorDesc", "fff");

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

	printf("\r\n feedback to server ,strSend:%s  LEN:%\d",strSend,strlen(strSend));
	aiwacFree(strSend);




}


/**************************************************************************
函数功能：控制小车到货物点
入口参数：无
返回  值：无
**************************************************************************/
void controlCarToGoodsSpace(void)
{
	int goalSide = 0;
	
	if (strcmp(GoodsLocation.side, "A") == 0)
		{
			goalSide = 1;
		}
	else if (strcmp(GoodsLocation.side, "B") == 0)
		{
			goalSide = 2;
		}
	else if(strcmp(GoodsLocation.side, "C") == 0) 
		{
			goalSide = 3;
		}

	if (goalSide == 0)
		{
			while(1)
				{
					delay_ms(1000);
					printf("\r\n controlCarToGoodsSpace: goalSide  error!!!");
				}
		}


	printf("\r\n controlCarToGoodsSpace:	goalSide:%d, LocationNow:%d",goalSide, LocationNow);
	
	goToEverywhere(goalSide, LocationNow, atof(GoodsLocation.distance));
	
	LocationNow = goalSide;

}


/**************************************************************************
函数功能：通知取货单元，商品 高度和深度
入口参数：无
返回  值：无
**************************************************************************/
void notifyGoodsGetterLocation(void )
{
	u16 jsonSize;
	cJSON *root;
	char *strJson;
	u8 strSend[300];

	memset(strSend, 0, sizeof(strSend));
	strSend[0] = '#';
	strSend[1] = '!';

	root=cJSON_CreateObject();
	cJSON_AddStringToObject(root,"businessType", "0014");
	cJSON_AddNumberToObject(root,"Height", atof(GoodsLocation.height));
	cJSON_AddNumberToObject(root,"Depth",  atof(GoodsLocation.depth));

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

	printf("\r\n notifyGoodsGetterLocation:%s",strSend);


}


/**************************************************************************
函数功能：等待取货单元反馈取到货
入口参数：无
返回  值：无
**************************************************************************/
void waitingGetterGotGoods(void)
{
	GotGoodsResult = 666;
	printfNUM = 0;
	
	while(1)
	{
		delay_ms(100);
		if (GotGoodsResult != 666)
			{
				break;
			}
		
		printfNUM++;
		if (printfNUM == 10)
			{
				printf("\r\n wating to geting  Goods")
			}
		
	}
	delay_ms(100);
	
	printf("\r\n waitingGetterGotGoods,result:%d",GotGoodsResult);

}


/**************************************************************************
函数功能：向服务端反馈取货情况
入口参数：无
返回  值：无
**************************************************************************/
void feedbackGotGoodsResult(void)
{

	cJSON *root, *data;  // 

	int num = 0;
	int numS = 0;

	char* strSend;
	char send[200];

	//	给服务器发状态
	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0005");
	cJSON_AddItemToObject(root, "data", data=cJSON_CreateObject());
	
	if (GotGoodsResult == 0)
		{
			cJSON_AddStringToObject(data,"status", "2");
		}
	else
		{
			cJSON_AddStringToObject(data,"status", "0");
		}
	
	cJSON_AddStringToObject(data,"errorDesc", "fff");

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

	printf("\r\n feedback to server ,strSend:%s  LEN:%\d",strSend,strlen(strSend));
	aiwacFree(strSend);

}



/**************************************************************************
函数功能：控制小车到放货点
入口参数：无
返回  值：无
**************************************************************************/
void controlCarToGate(void)
{
	int goalSide = 0;
	goalSide = 1;  // 出货门 在A  -> 1



	printf("\r\n controlCarToGate:	goalSide:%d, LocationNow:%d",goalSide, LocationNow);
	
	goToEverywhere(goalSide, LocationNow, DROP_GOODS_SPACE);
	
	LocationNow = goalSide;


}


/**************************************************************************
函数功能：通知取货单元放货
入口参数：无
返回  值：无
**************************************************************************/
void notifyGoodsGetterLoseGoods(void )
{
	u16 jsonSize;
	cJSON *root;
	char *strJson;
	u8 strSend[300];

	memset(strSend, 0, sizeof(strSend));
	strSend[0] = '#';
	strSend[1] = '!';

	root=cJSON_CreateObject();
	cJSON_AddStringToObject(root,"businessType", "0018");

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

	printf("\r\n notifyGoodsGetterLocation:%s",strSend);


}

/**************************************************************************
函数功能：等待取货单元反馈放货
入口参数：无
返回  值：无
**************************************************************************/
void waitingGetterLoseGoods(void)
{
	LoseGoodsResult = 666;

	printfNUM = 0;
	
	while(1)
	{
		delay_ms(100);
		if (LoseGoodsResult != 666)
			{
				break;
			}
		
		printfNUM++;
		if (printfNUM == 10)
			{
				printf("\r\n wating losing Goods")
			}
	}
	delay_ms(100);
	
	printf("\r\n waitingGetterGotGoods,result:%d",LoseGoodsResult);

}




/**************************************************************************
函数功能：向服务端反馈放货情况
入口参数：无
返回  值：无
**************************************************************************/
void feedbackLoseGoodsResult(void)
{

	cJSON *root, *data;  // 

	int num = 0;
	int numS = 0;

	char* strSend;
	char send[200];

	//	给服务器发状态
	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0006");
	cJSON_AddItemToObject(root, "data", data=cJSON_CreateObject());
	
	if (LoseGoodsResult == 0)
		{
			cJSON_AddStringToObject(data,"status", "3");
		}
	else
		{
			cJSON_AddStringToObject(data,"status", "0");
		}
	
	cJSON_AddStringToObject(data,"errorDesc", "fff");

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

	printf("\r\n feedback to server ,strSend:%s  LEN:%\d",strSend,strlen(strSend));
	aiwacFree(strSend);

}


/**************************************************************************
函数功能：控制小车到丢盘子的地方
入口参数：无
返回  值：无
**************************************************************************/
void controlCarToDropPan(void)
{
	int goalSide = 0;
	goalSide = 1;  // 丢盘子 在A  -> 1

	printf("\r\n controlCarToDropPan:	goalSide:%d, LocationNow:%d",goalSide, LocationNow);
	
	goToEverywhere(goalSide, LocationNow, DROP_PAN_SPACE);
	
	LocationNow = goalSide;

}


/**************************************************************************
函数功能：通知取货单元，丢盘子
入口参数：无
返回  值：无
**************************************************************************/
void notifyGoodsGetterDropPan(void )
{
	u16 jsonSize;
	cJSON *root;
	char *strJson;
	u8 strSend[300];

	memset(strSend, 0, sizeof(strSend));
	strSend[0] = '#';
	strSend[1] = '!';

	root=cJSON_CreateObject();
	cJSON_AddStringToObject(root,"businessType", "0018");

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

	printf("\r\n notifyGoodsGetterLocation:%s",strSend);


}

/**************************************************************************
函数功能：等待取货单元丢盘子
入口参数：无
返回  值：无
**************************************************************************/
void waitingGetterLosePan(void)
{
	LosePanResult = 666;

	printfNUM = 0;
	
	while(1)
	{
		delay_ms(100);
		if (LosePanResult != 666)
			{
				break;
			}
		
		printfNUM++;
		if (printfNUM == 10)
			{
				printf("\r\n wating losing Pan")
			}
	}
	delay_ms(100);
	
	printf("\r\n waitingGetterGotGoods,result:%d",LosePanResult);

}



/**************************************************************************
函数功能：控制小车到复位点
入口参数：无
返回  值：无
**************************************************************************/
void controlCarToInitSpace(void)
{
	int goalSide = 0;
	goalSide = 2;  // 复位点 在B  -> 2



	printf("\r\n controlCarToInitSpace:	goalSide:%d, LocationNow:%d",goalSide, LocationNow);
	
	goToEverywhere(goalSide, LocationNow, CAR_INIT_SPACE);
	
	LocationNow = goalSide;


}

/**************************************************************************
函数功能：向服务端反馈复位情况
入口参数：无
返回  值：无
**************************************************************************/
void feedbackGoInit(void)
{

	cJSON *root, *data;  // 

	int num = 0;
	int numS = 0;

	char* strSend;
	char send[200];

	//	给服务器发状态
	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0002");
	cJSON_AddItemToObject(root, "data", data=cJSON_CreateObject());
	cJSON_AddStringToObject(data,"status", "4");
	cJSON_AddStringToObject(data,"errorDesc", "fff");

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

	printf("\r\n feedback to server ,strSend:%s  LEN:%\d",strSend,strlen(strSend));
	aiwacFree(strSend);

}


/**************************************************************************
函数功能：	指定方向，运动到某方向剩余的距离处
入口参数：	 direction：方向    		needDistance：剩余的距离
返回  值：		无
**************************************************************************/
void goToLocation(int direction,double needDistance)
{

	//起步阶段，需要两车平行起步
	goStartTogether(direction);

	// 运动到某方向的，指定地点
	goGoalPosition( direction,needDistance);

}



/**************************************************************************
函数功能：	两车起步的逻辑，在指定方向让后面的车 移动到前面，然后等矫正
入口参数： int direction  方向 
返回  值：		无
**************************************************************************/
void goStartTogether(int direction)
{
	double goalLocation = 0 ;
	double TogetherGap = 0.02;

	while(( Car1_moveState > 1 )|| (Car2_moveState > 1) ) //  当前有  小车在  转弯
	{
		printf("\r\nwaiting for turing,  Car1_moveState :%d,  Car2_moveState:%d ",Car1_moveState ,Car2_moveState );
		delay_ms(50); //   等待转完
	}



	AiwacMasterSendOrderCar1(CAR_STOP , STATE_STOP) ;
	AiwacMasterSendOrderCar2(CAR_STOP , STATE_STOP) ;
	delay_ms(1000);

	while (1)
		{
			if ((Car1_FDistance>0) && (Car2_FDistance>0))  //未获取到前方距离
				{

					break;
				}

			delay_ms(50);
			printf("\r\n waiting	goStartTogether get FDistance");

		}


	

	//等校正
	while (1) 
		{
			delay_ms(50);
			
			if (  (Car1_CorrectState  == 1) && ( Car2_CorrectState == 1) )//姿态校准  ok
			{
				
				break;
			}
			printf("\r\n waiting	goStartTogether correction ");

		}
	
	printf("\r\n step1	goStartTogether correction ok");
	

	
	//往前走
	if (direction == FRONT_DIRECTION)
		{

			if(Car1_FDistance <Car2_FDistance)
				{

						goalLocation = Car1_FDistance;
						printf("\r\n step1	goStartTogether : Car1_FDistance  is goal ");
				}
			else
				{
						goalLocation = Car2_FDistance;
						printf("\r\n step1	goStartTogether : Car2_FDistance  is goal ");
				}

				
			if (myabs_double(Car1_FDistance- goalLocation) <= TogetherGap)  // 车1是标准位置
				{
					printf("\r\n goStartTogether:study from car1 ");
					
					while (1)
					{
						delay_ms(80);
	

						if (myabs_double(Car2_FDistance- goalLocation) <= TogetherGap) // 车2 ok
							{
								printf("\r\n goStartTogether:CorrectState   ok");
								AiwacMasterSendOrderCar2(CAR_STOP , STATE_STOP) ;
								break;
							}

						
						if( (Car2_FDistance)< goalLocation - TogetherGap) //走超了
							{
								AiwacMasterSendOrderCar2(-MIN_SPEED , STATE_STRAIGHT) ;
								printf("\r\n goStartTogether:over");

							}
						else if ((Car2_FDistance)> 4*TogetherGap+ goalLocation) //还较远
							{

								AiwacMasterSendOrderCar2(4*MIN_SPEED , STATE_STRAIGHT) ; 
								printf("\r\n goStartTogether:too far");
							}
						else if ((Car2_FDistance)> TogetherGap+ goalLocation) //较近
							{
								AiwacMasterSendOrderCar2(MIN_SPEED , STATE_STRAIGHT) ; 
								printf("\r\n goStartTogether: far");
							}

					}
				}
			else
				{
					printf("\r\ngoStartTogether: study from car2 ");
					while (1)
					{
						delay_ms(80);

						printf("\r\n Car1:Car1_CorrectState :%d,  Car1_FDistance:%f,   Car1_moveState:%d",Car1_CorrectState ,Car1_FDistance, Car1_moveState);
						printf("\r\n Car2:Car2_CorrectState :%d,  Car2_FDistance:%f,   Car2_moveState:%d",Car2_CorrectState ,Car2_FDistance, Car2_moveState);

						if (myabs_double(Car1_FDistance- goalLocation) <= TogetherGap) // 车1 ok
							{
								printf("\r\n goStartTogether:CorrectState   ok");
								AiwacMasterSendOrderCar1(CAR_STOP , STATE_STOP) ;
								break;
							}

						
						if( (Car1_FDistance)< goalLocation - TogetherGap) //走超了
							{
								AiwacMasterSendOrderCar1(-MIN_SPEED , STATE_STRAIGHT) ;
								printf("\r\n goStartTogether:over");

							}
						else if ((Car1_FDistance)> 4*TogetherGap+ goalLocation) //还较远
							{

								AiwacMasterSendOrderCar1(4*MIN_SPEED , STATE_STRAIGHT) ; 
								printf("\r\n goStartTogether:too far");
							}
						else if ((Car1_FDistance)> TogetherGap+ goalLocation) //较近
							{
								AiwacMasterSendOrderCar1(MIN_SPEED , STATE_STRAIGHT) ; 
								printf("\r\n goStartTogether: far");
							}
	

					}


				}


		}
	else  // 往后走
		{

			if(Car1_BDistance <Car2_BDistance)
			{

					goalLocation = Car1_BDistance;
					printf("\r\n step1	goStartTogether : Car1_BDistance  is goal ");
			}
			else
			{
					goalLocation = Car2_BDistance;
					printf("\r\n step1	goStartTogether : Car2_BDistance  is goal ");
			}

					
			if (myabs_double(Car1_BDistance- goalLocation) <= TogetherGap)  // 车1是标准位置
			{
				printf("\r\n goStartTogether:study from car1 ");
				
				while (1)
				{
					delay_ms(80);


					if (myabs_double(Car2_BDistance- goalLocation) <= TogetherGap) // 车2 ok
						{
							printf("\r\n goStartTogether:CorrectState   ok");
							AiwacMasterSendOrderCar2(CAR_STOP , STATE_STOP) ;
							break;
						}

					
					if( (Car2_BDistance)< goalLocation - TogetherGap) //走超了
						{
							AiwacMasterSendOrderCar2(MIN_SPEED , STATE_STRAIGHT) ;
							printf("\r\n goStartTogether:over");

						}
					else if ((Car2_BDistance)> 4*TogetherGap+ goalLocation) //还较远
						{

							AiwacMasterSendOrderCar2(-4*MIN_SPEED , STATE_STRAIGHT) ; 
							printf("\r\n goStartTogether:too far");
						}
					else if ((Car2_BDistance)> TogetherGap+ goalLocation) //较近
						{
							AiwacMasterSendOrderCar2(-MIN_SPEED , STATE_STRAIGHT) ; 
							printf("\r\n goStartTogether:far");
						}

				}
			}
			else
			{
				printf("\r\ngoStartTogether: study from car2 ");
				while (1)
				{
					delay_ms(80);
/*
					printf("\r\n Car1:Car1_CorrectState :%d,  Car1_FDistance:%f,   Car1_moveState:%d",Car1_CorrectState ,Car1_FDistance, Car1_moveState);
					printf("\r\n Car2:Car2_CorrectState :%d,  Car2_FDistance:%f,   Car2_moveState:%d",Car2_CorrectState ,Car2_FDistance, Car2_moveState);
*/
					if (myabs_double(Car1_BDistance- goalLocation) <= TogetherGap) // 车1 ok
						{
							printf("\r\n goStartTogether:CorrectState   ok");
							AiwacMasterSendOrderCar1(CAR_STOP , STATE_STOP) ;
							break;
						}

					
					if( (Car1_BDistance)< goalLocation - TogetherGap) //走超了
						{
							AiwacMasterSendOrderCar1(MIN_SPEED , STATE_STRAIGHT) ;
							printf("\r\n goStartTogether:over");

						}
					else if ((Car1_BDistance)> 4*TogetherGap+ goalLocation) //还较远
						{

							AiwacMasterSendOrderCar1(-4*MIN_SPEED , STATE_STRAIGHT) ; 
							printf("\r\n goStartTogether:too far");
						}
					else if ((Car1_BDistance)> TogetherGap+ goalLocation) //较近
						{
							AiwacMasterSendOrderCar1(-MIN_SPEED , STATE_STRAIGHT) ; 
							printf("\r\n goStartTogether: far");
						}


				}


			}
		}
		delay_ms(60);

		AiwacMasterSendOrderCar1(CAR_STOP , STATE_STOP) ;
		AiwacMasterSendOrderCar2(CAR_STOP , STATE_STOP) ;
		delay_ms(1000);




	//等校正
	while (1) 
		{
			delay_ms(50);
			
			if (  (Car1_CorrectState  == 1) && ( Car2_CorrectState == 1) )//姿态校准  ok
			{
				printf("\r\n step3  goStartTogether correction ok ");
				break;
			}

		}


}





/**************************************************************************
函数功能：	两车已经平行，在指定方向运动到目标地点
入口参数： int direction  方向 
返回  值：		无
**************************************************************************/
void goGoalPosition(int direction,double NeedDistance)
{
	double goalGAP = 0.015;   //m
	double iniTDistance = 0; // 起步距离，用于 渐进起步
	double needDistance = NeedDistance;
	

	
	AiwacMasterSendOrderCar1(CAR_STOP , STATE_STOP) ;
	AiwacMasterSendOrderCar2(CAR_STOP , STATE_STOP) ;

	delay_ms(300);

	while (1)
		{
			if ((Car1_FDistance>0) && (Car2_FDistance>0))  //为获取到前方距离
				{

					break;
				}

			delay_ms(50);

		}
	

	//等校正
	while (1) 
		{
			delay_ms(80);
			
			if (  (Car1_CorrectState  == 1) && ( Car2_CorrectState == 1) )//姿态校准  ok
			{
				printf("\r\n step1  goGoalPosition correction ok ");
				break;
			}

		}


	
	// 运动到目标地点
	if (direction == FRONT_DIRECTION)
		{

			iniTDistance = (Car2_FDistance + Car1_FDistance)/2;

			while(1)
				{
					delay_ms(50);

					printf("\r\n Car1:Car1_CorrectState :%d,  Car1_FDistance:%f,   Car1_moveState:%d",Car1_CorrectState ,Car1_FDistance, Car1_moveState);
					printf("\r\n Car2:Car2_CorrectState :%d,  Car2_FDistance:%f,   Car2_moveState:%d",Car2_CorrectState ,Car2_FDistance, Car2_moveState);
								


				
					if (( ( myabs_double(Car1_FDistance- needDistance ) <  goalGAP*10 ) || ( ( myabs_double(Car2_FDistance- needDistance ) <  goalGAP*10 )) )

					||((Car1_FDistance- needDistance )<0)  || (Car2_FDistance- needDistance ) <0)
						{

							// 到达目的位置
							if ( ( myabs_double(Car1_FDistance- needDistance ) <  goalGAP ) && ( ( myabs_double(Car2_FDistance- needDistance ) <  goalGAP )) )
								{


									AiwacMasterSendOrderCar1(CAR_STOP , STATE_STOP) ;
									AiwacMasterSendOrderCar2(CAR_STOP , STATE_STOP) ;
									delay_ms(200);
									break;

								}


							
						
							// 车1 的情况
							if (Car1_FDistance >= (needDistance + goalGAP))
								{
								
									AiwacMasterSendOrderCar1(MIN_SPEED , STATE_STRAIGHT) ;
									printf("\r\ncar1  go on");
								}
							else if (Car1_FDistance <= (needDistance-goalGAP) )
								{
									AiwacMasterSendOrderCar1(-MIN_SPEED , STATE_STRAIGHT) ;
									printf("\r\ncar1  go back");
								}
							
							else{
									AiwacMasterSendOrderCar1(CAR_STOP , STATE_STOP) ;
									printf("\r\ncar1  wait for turing order");
								}


							
						
						// 车2 的情况
							if (Car2_FDistance >= (needDistance +goalGAP))
								{
								
									AiwacMasterSendOrderCar2(MIN_SPEED , STATE_STRAIGHT) ;
									printf("\r\ncar2  go on");
								}
							else if (Car2_FDistance <= (needDistance-goalGAP ))
								{
									AiwacMasterSendOrderCar2(-MIN_SPEED , STATE_STRAIGHT) ;
									printf("\r\ncar2  go back");
								}
								else{
									AiwacMasterSendOrderCar2(CAR_STOP , STATE_STOP) ;
									printf("\r\ncar2  wait for turing order");
								}


						}
					else // 未到目标位置
						{
							if( myabs_double(Car1_FDistance - Car2_FDistance ) < goalGAP*4)  //  两车的	前进 距离ok
							{
								// 下发  继续 默认前进 
								AiwacMasterSendOrderCar1(designFSpeed2((Car1_FDistance+Car2_FDistance)/2, needDistance,iniTDistance) , STATE_STRAIGHT) ;
								AiwacMasterSendOrderCar2(designFSpeed2((Car1_FDistance+Car2_FDistance)/2, needDistance,iniTDistance) , STATE_STRAIGHT) ;
								printf("\r\ngo on straight");
							}
							else // 两车的  前进 距离  no
							{
								if (Car1_FDistance - Car2_FDistance >0)  // 1车在后 
									{
										// 发送 2车默认速度，1车 比默认快点
										AiwacMasterSendOrderCar1(designFSpeed2(Car1_FDistance, needDistance,iniTDistance)  +MIN_SPEED*2, STATE_STRAIGHT) ;
										AiwacMasterSendOrderCar2((designFSpeed2(Car2_FDistance, needDistance,iniTDistance) ), STATE_STRAIGHT) ;
										printf("\r\n car1 needs to go fast");
									}
								else
									{
										// 发送 1车默认速度，2车 比默认快点
										AiwacMasterSendOrderCar1(designFSpeed2(Car1_FDistance, needDistance,iniTDistance) , STATE_STRAIGHT) ;
										AiwacMasterSendOrderCar2(designFSpeed2(Car2_FDistance, needDistance,iniTDistance)+ MIN_SPEED*2 , STATE_STRAIGHT) ;
										printf("\r\n car2 needs to go fast");
									}
								
							}

						}
				}

	}
	else		//后面的值
		{

		iniTDistance = (Car2_BDistance + Car1_BDistance)/2;
		
		while(1)
			{
				delay_ms(50);

				printf("\r\n Car1:Car1_CorrectState :%d,  Car1_BDistance:%f,   Car1_moveState:%d",Car1_CorrectState ,Car1_BDistance, Car1_moveState);
				printf("\r\n Car2:Car2_CorrectState :%d,  Car2_BDistance:%f,   Car2_moveState:%d",Car2_CorrectState ,Car2_BDistance, Car2_moveState);
							


			
				if (( ( myabs_double(Car1_BDistance- needDistance ) <  goalGAP*10 ) || ( ( myabs_double(Car2_BDistance- needDistance ) <  goalGAP*10 )) )

				||((Car1_BDistance- needDistance )<0)  || (Car2_BDistance- needDistance ) <0)
					{

						// 到达目的位置
						if ( ( myabs_double(Car1_BDistance- needDistance ) <  goalGAP ) && ( ( myabs_double(Car2_BDistance- needDistance ) <  goalGAP )) )
							{


								AiwacMasterSendOrderCar1(CAR_STOP , STATE_STOP) ;
								AiwacMasterSendOrderCar2(CAR_STOP , STATE_STOP) ;
								delay_ms(200);
								break;

							}


						
					
						// 车1 的情况
						if (Car1_BDistance >= (needDistance + goalGAP))
							{
							
								AiwacMasterSendOrderCar1(-MIN_SPEED , STATE_STRAIGHT) ;
								printf("\r\ncar1  go on");
							}
						else if (Car1_BDistance <= (needDistance-goalGAP) )
							{
								AiwacMasterSendOrderCar1(MIN_SPEED , STATE_STRAIGHT) ;
								printf("\r\ncar1  go back");
							}
						
						else{
								AiwacMasterSendOrderCar1(CAR_STOP , STATE_STOP) ;
								printf("\r\ncar1  wait for turing order");
							}


						
					
					// 车2 的情况
						if (Car2_BDistance >= (needDistance +goalGAP))
							{
							
								AiwacMasterSendOrderCar2(-MIN_SPEED , STATE_STRAIGHT) ;
								printf("\r\ncar2  go on");
							}
						else if (Car2_BDistance <= (needDistance-goalGAP ))
							{
								AiwacMasterSendOrderCar2(MIN_SPEED , STATE_STRAIGHT) ;
								printf("\r\ncar2  go back");
							}
							else{
								AiwacMasterSendOrderCar2(CAR_STOP , STATE_STOP) ;
								printf("\r\ncar2  wait for turing order");
							}


					}
				else // 未到目标位置
					{
						if( myabs_double(Car1_BDistance - Car2_BDistance ) < goalGAP*4)  //  两车的	前进 距离ok
						{
							// 下发  继续 默认前进 
							AiwacMasterSendOrderCar1(-(designFSpeed2((Car1_BDistance+Car2_BDistance)/2, needDistance,iniTDistance)) , STATE_STRAIGHT) ;
							AiwacMasterSendOrderCar2(-(designFSpeed2((Car1_BDistance+Car2_BDistance)/2, needDistance,iniTDistance)) , STATE_STRAIGHT) ;
							printf("\r\ngo on straight");
						}
						else // 两车的  前进 距离  no
						{
							if (Car1_BDistance - Car2_BDistance >0)  // 1车在后 
								{
									// 发送 2车默认速度，1车 比默认快点
									AiwacMasterSendOrderCar1(-(designFSpeed2(Car1_BDistance, needDistance,iniTDistance)  +MIN_SPEED*2), STATE_STRAIGHT) ;
									AiwacMasterSendOrderCar2(-(designFSpeed2(Car2_BDistance, needDistance,iniTDistance) ), STATE_STRAIGHT) ;
									printf("\r\n car1 needs to go fast");
								}
							else
								{
									// 发送 1车默认速度，2车 比默认快点
									AiwacMasterSendOrderCar1(-(designFSpeed2(Car1_BDistance, needDistance,iniTDistance) ), STATE_STRAIGHT) ;
									AiwacMasterSendOrderCar2(-(designFSpeed2(Car2_BDistance, needDistance,iniTDistance)+ MIN_SPEED*2) , STATE_STRAIGHT) ;
									printf("\r\n car2 needs to go fast");
								}
							
						}

					}
			}

		}





	AiwacMasterSendOrderCar1(CAR_STOP , STATE_STOP) ;
	AiwacMasterSendOrderCar2(CAR_STOP , STATE_STOP) ;
	delay_ms(50);

		//等校正
	while (1) 
		{
			delay_ms(50);
			
			if (  (Car1_CorrectState  == 1) && ( Car2_CorrectState == 1) )//姿态校准  ok
			{
				printf("\r\n step3  goGoalPosition correction ok ");
				break;
			}

		}


}



void sendTuringOrder(int Left_or_Right)
{

	//	转弯的  方向 要看 在 超市哪边
	AiwacMasterSendOrderCar1(CAR_STOP , Left_or_Right) ;
	AiwacMasterSendOrderCar2(CAR_STOP , Left_or_Right) ;
	delay_ms(120);

	//若未进入转弯
	while ((Car2_moveState <2 ) || (Car1_moveState <2 ) )  //有车未转弯
		{

			
			if 	(Car1_moveState <2 )
				{
						AiwacMasterSendOrderCar1(CAR_STOP , Left_or_Right) ;
				}

						
			if 	(Car2_moveState <2 )
				{
						AiwacMasterSendOrderCar2(CAR_STOP , Left_or_Right) ;
				}
			delay_ms(70);
			
		}

	
	// 检查  是否结束
	while((( Car1_moveState > 1 )|| (Car2_moveState > 1) ))
		{

			printf("\r\nwaiting for turing,  Car1_moveState :%d,  Car2_moveState:%d ",Car1_moveState ,Car2_moveState );
			delay_ms(10);
		}

	
	printf("\r\nturing  over");
}




/**************************************************************************
函数功能：	根据前方距离 定小车前进速度
入口参数：	 前方
返回  值：		前方速度
**************************************************************************/
double  designFSpeed2(double FD, double FD_care,double iniTDistance)
{
	double FSpeed = 30;		// 低速的速度 mm

	double FDSMax = FD_MAX_SPEED;  // 规定的最大  前方速度  mm

	double startSpeed = 0;
	FD_care = FD_care + 0.10 ;	// 前方警戒距离，需要  低速前进



	if ((iniTDistance >=FD) || (iniTDistance -FD)*1000 <100)
		{
			startSpeed = (iniTDistance -FD)*1000*2+FSpeed;
	
		}

	
	if (FD>FD_care)  // 离危险距离较远
	{
		FSpeed = (FD - FD_care)*700 + FSpeed;
	}


	if (startSpeed >0)
		FSpeed = (FSpeed>startSpeed) ? startSpeed:FSpeed;

	
	if (FSpeed > FDSMax)
	{
		FSpeed = FDSMax;
	}


	return FSpeed;

}


/**************************************************************************
函数功能：	给小车1发 速度 和 小车的运动状态指令 
入口参数：	 X_V  : X轴的速度,前进速度			     moveState：小车的运动状态指令
返回  值：		无
**************************************************************************/
void AiwacMasterSendOrderCar1(double X_V, int moveState)
{
	u16 jsonSize;
	cJSON *root;
	char *strJson;
	char strSend[100];
	
	strSend[0] = '#';
	strSend[1] = '!';


	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0009");
	cJSON_AddNumberToObject(root,"X_V", X_V);
	cJSON_AddNumberToObject(root,"mo", moveState);


	//strJson=cJSON_Print(root); 
	//printf("\r\n cJSON_Print Len:%d",strlen(strJson));

	strJson  =cJSON_PrintUnformatted(root);

	//printf("\r\n cJSON_PrintUnformatted Len:%d",strlen(strJson));
	
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


}



/**************************************************************************
函数功能：	给小车2发 速度 和 小车的运动状态指令 
入口参数：	 X_V  : X轴的速度,前进速度			     moveState：小车的运动状态指令
返回  值：		无
**************************************************************************/
void AiwacMasterSendOrderCar2(double X_V, int moveState)
{
	u16 jsonSize;
	cJSON *root;
	char *strJson;
	u8 strSend[100];
	
	strSend[0] = '#';
	strSend[1] = '!';


	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0010");
	cJSON_AddNumberToObject(root,"X_V", X_V);
	cJSON_AddNumberToObject(root,"mo", moveState);


	//strJson=cJSON_Print(root); 
	strJson  =cJSON_PrintUnformatted(root);
	cJSON_Delete(root); 
	
	jsonSize = strlen(strJson);

	strSend[2] = jsonSize >> 8;
	strSend[3] = jsonSize;

	strncpy(strSend+4,strJson,jsonSize);


		
	strSend[jsonSize+4] = '*';
	strSend[jsonSize+5] = crc8_calculate(strJson, jsonSize);
	strSend[jsonSize+6] = '&';
	// 需要打开
	uart5_sendString(strSend,7 + jsonSize);
	myfree(strJson);


}



/**************************************************************************
函数功能：	给定当前位置 和   目标位置 控制小车运动
入口参数：goalSide：目标边   ,		nowSide：当前边   	goDistance；按图的距离
返回  值：		无
**************************************************************************/
void goToEverywhere(int goalSide,int nowSide, double goDistance)
{
	//按分区进行  主控逻辑
	if (strcmp("Area1", MASTER_ID)  == 0)
	{



		// 当前在A 
		
		if ((nowSide == 1))
		{
			// 在A,去A
			if (goalSide == 1)		
			{
				if (Car1_FDistance >= goDistance)
				{
					goToLocation(FRONT_DIRECTION, goDistance);
				}
				else
				{
					goToLocation(BACK_DIRECTION, A_HALF_LEN - goDistance);
				}

				return;
			}
				
			// 在A,去B
			if (goalSide == 2)		
			{
				goToLocation(BACK_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_LEFT);
				goToLocation(BACK_DIRECTION, B_LEN - goDistance);

				return;
			}
			
			// 在A,去c
			if (goalSide == 3)	
			{
				goToLocation(BACK_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_LEFT);
				goToLocation(BACK_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_LEFT);
				goToLocation(BACK_DIRECTION, C_HALF_LEN - goDistance);
				return;
			}

		}
		


		// 当前在B 
		if (nowSide == 2)
		{
			// 在B,去A
			if (goalSide == 1) 		
			{
				goToLocation(FRONT_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_RIGHT);
				goToLocation(FRONT_DIRECTION, goDistance);
				return;
			
			}
			
			// 在B,去B
			if (goalSide == 2) 		
			{
				if (Car1_FDistance >= goDistance)
					{
						goToLocation(FRONT_DIRECTION, goDistance);
					}
				else
					{
						goToLocation(BACK_DIRECTION, B_LEN - goDistance);
					}

				return;

			}

			// 在B,去C
			if (goalSide == 3) 		
			{
				goToLocation(BACK_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_LEFT);
				goToLocation(BACK_DIRECTION, C_HALF_LEN - goDistance);
				return;
			}

		}



		// 当前在C 
		if (nowSide == 3)
		{
			
			// 在C,去A
			if (goalSide == 1) 		
			{
				goToLocation(FRONT_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_RIGHT);
				goToLocation(FRONT_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_RIGHT);
				goToLocation(FRONT_DIRECTION, goDistance);
				
				return;
			}

			// 在C,去B
			if (goalSide == 2) 		
			{
				goToLocation(FRONT_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_RIGHT);
				goToLocation(FRONT_DIRECTION, goDistance);

				return;
			}

			// 在C,去C
			if (goalSide == 3) 		
			{
				if (Car1_FDistance >= goDistance)
					{
						goToLocation(FRONT_DIRECTION, goDistance);
					}
				else
					{
						goToLocation(BACK_DIRECTION, C_HALF_LEN - goDistance);
					}
				return;
				
			}

		}

	}
	else	// Area2  master
	{
		// 当前在A 
		if ((nowSide == 1))
		{
			// 在A,去A
			if (goalSide == 1)		
			{
				if (Car1_BDistance >= goDistance)
				{
					goToLocation(BACK_DIRECTION, goDistance);
				}
				else
				{
					goToLocation(FRONT_DIRECTION, A_HALF_LEN - goDistance);
				}
				return;
			}
				
			// 在A,去B
			if (goalSide == 2)		
			{
				goToLocation(FRONT_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_RIGHT);
				goToLocation(FRONT_DIRECTION, B_LEN - goDistance);

				return;
			}
			
			// 在A,去c
			if (goalSide == 3)	
			{
				goToLocation(FRONT_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_RIGHT);
				goToLocation(FRONT_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_RIGHT);
				goToLocation(FRONT_DIRECTION, C_HALF_LEN - goDistance);

				return;
			}

		}
		


		// 当前在B 
		if (nowSide == 2)
		{
			// 在B,去A
			if (goalSide == 1) 		
			{
				goToLocation(BACK_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_LEFT);
				goToLocation(BACK_DIRECTION, goDistance);
				return;
			}
			
			// 在B,去B
			if (goalSide == 2) 		
			{
				if (Car1_BDistance >= goDistance)
					{
						goToLocation(BACK_DIRECTION, goDistance);
					}
				else
					{
						goToLocation(FRONT_DIRECTION, B_LEN - goDistance);
					}
				return;

			}

			// 在B,去C
			if (goalSide == 3) 		
			{
				goToLocation(FRONT_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_RIGHT);
				goToLocation(FRONT_DIRECTION, C_HALF_LEN - goDistance);
				return;
			}

		}



		// 当前在C 
		if (nowSide == 3)
		{
			
			// 在C,去A
			if (goalSide == 1) 		
			{
				goToLocation(FRONT_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_RIGHT);
				goToLocation(FRONT_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_RIGHT);
				goToLocation(FRONT_DIRECTION, goDistance);
				return;
			
			}

			// 在C,去B
			if (goalSide == 2) 		
			{
				goToLocation(BACK_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_LEFT);
				goToLocation(BACK_DIRECTION, goDistance);
				return;
			}

			// 在C,去C
			if (goalSide == 3) 		
			{
				if (Car1_BDistance >= goDistance)
					{
						goToLocation(BACK_DIRECTION, goDistance);
					}
				else
					{
						goToLocation(FRONT_DIRECTION, C_HALF_LEN - goDistance);
					}
				return;
			}

		}
	}



}


