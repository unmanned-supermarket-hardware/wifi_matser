#include "AIWAC_Supermarket.h"



//WIFI STA模式,设置要去连接的路由器无线参数,请根据你自己的路由器设置,自行修改.
const u8* wifista_ssid="redmi";			//路由器SSID�?
const u8* wifista_encryption="WPA";	//wpa/wpa2 aes加密方式
const u8* wifista_password="tangyuan"; 	//连接密码

//连接端口�?8086,可自行修改为其他端口.
const u8* portnum="8890";	


struct systemState SystemState;
int  printfNUM = 0	;//打印的计�?
struct goodsLocation GoodsLocation;

int GotGoodsResult ;		// 取货结果  "Result": int类型, 0 表示成功�?1表示失败
int LoseGoodsResult ;		// 丢货结果  "Result": int类型, 0 表示成功�?1表示失败
int LosePanResult ;			// 丢盘结果  "Result": int类型, 0 表示成功�?1表示失败


int LocationNow = 2;	//A:1  B:2  C:3



// 小车的情�?
int Car1_CorrectState = -1;
double Car1_FDistance = -1;
double Car1_BDistance = -1;
int Car1_moveState = -1;

int Car2_CorrectState = -1;
double Car2_FDistance = -1;
double Car2_BDistance = -1;
int Car2_moveState = -1;

int netTime = 0;

void initSysValue(void)
{	
	memset((void*)(&SystemState), 0, sizeof(SystemState));
	memset((void*)(&GoodsLocation), 0, sizeof(GoodsLocation));
	
	GotGoodsResult = 666;		// 取货结果  "Result": int类型, 0 表示成功�?1表示失败
	LoseGoodsResult = 666;		// 丢货结果  "Result": int类型, 0 表示成功�?1表示失败
	LosePanResult = 666;		// 丢盘结果  "Result": int类型, 0 表示成功�?1表示失败

	USART2_Car1_jsonParseBuF[0] = '-' ;
	USART4_Getter_jsonParseBuF[0] = '-' ;
	USART5_Car2_jsonParseBuF[0] = '-' ;



	// 小车的情�?
	Car1_CorrectState = -1;
	 Car1_FDistance = -1;
	Car1_BDistance = -1;
	Car1_moveState = -1;

	Car2_CorrectState = -1;
	Car2_FDistance = -1;
	Car2_BDistance = -1;
	Car2_moveState = -1;

	LocationNow = 2;  // 初始位置在B�?

	printfNUM = 0;

	printf("\r\n initSysValue  OK");
}





/**************************************************************************
函数功能：初始化wifi模块
入口参数：无
返回  值：�?
**************************************************************************/
void wifi_Init(void)
{
	printf("\r\n start wifi_Init");
	atk_8266_test();		//进入ATK_ESP8266
	delay_ms(200);
	atk_8266_at_response(1);
	printf("\r\n wifi_Init OK");
}




/**************************************************************************
函数功能：等待来自服务器的指定消息类型，解析再进行处�?
入口参数：goalType：目标等�? 类型
返回  值：�?
**************************************************************************/
void parseOrderFromS(int goalType)
{
	char getMS[200];
	cJSON *root, *orderValue,*data;  // 
	u16 rlen = 0;

	int businessType =999;


	float length = 0;	// 位置的各种信息
	
	while(1)
	{
		if(USART3_RX_STA&0X8000)		
		{ 
			rlen=USART3_RX_STA&0X7FFF;	//得到本次接收到的数据长度
			USART3_RX_BUF[rlen]=0;	
			//printf("\r\nlen:%d",rlen);

			netTime = 0;

			if (	(USART3_RX_BUF[0] == '#') 
				&&	(USART3_RX_BUF[1] == '!')
				&&	(USART3_RX_BUF[rlen-2] == '&')
				)
			{
				memset(getMS, 0, sizeof(getMS));
				
				strncpy(getMS, USART3_RX_BUF+2, rlen-4);   //  后面要测试下
				//printf("\r\n getMs:%s,getMslast:%c,,first:%c,len:%d,VS:%d",getMS,getMS[strlen(getMS)-1],getMS[0],strlen(getMS),strncmp("{\"businessType\": \"0001\" }",getMS, 1));
				
				root = cJSON_Parse(getMS);
				if (!root) 
				{
					printf("\r\nError before: [%s]\n",cJSON_GetErrorPtr());
					USART3_RX_STA = 0;
					continue;
				}


				orderValue = cJSON_GetObjectItem(root, "businessType");  //  ×?D￡�??é??
				if (!orderValue) {
					printf("get name faild !\n");
					printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					USART3_RX_STA = 0;
					cJSON_Delete(root);
					continue;
				}

				
				businessType = atoi(orderValue->valuestring);
				if (businessType == goalType)  //进行目标消息类型的处�?
					{
						if(goalType == 1)
							{
								printf("\r\n master gets businessTy:%d",businessType);
								printf("\r\n get the mesg form android  to ask master state");
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
								strcpy(GoodsLocation.side,orderValue->valuestring);

								orderValue = cJSON_GetObjectItem(data, "distance");
								if (!orderValue) {
									printf("get name faild !\n");
									printf("Error before: [%s]\n", cJSON_GetErrorPtr());
									USART3_RX_STA = 0;
									cJSON_Delete(root);
									continue;
								}	

								printf("\r\norderValue->valuestring:%s,double:%f,double/1000:%f",orderValue->valuestring,atof(orderValue->valuestring), atof(orderValue->valuestring)/1000);
								length =  atof(orderValue->valuestring)/1000;
								sprintf(GoodsLocation.distance,"%f",length);
								printf("\r\GoodsLocation.distance:%s",GoodsLocation.distance);
								//strcpy(GoodsLocation.distance,orderValue->valuestring);
								//printf("\r\n")

								orderValue = cJSON_GetObjectItem(data, "height");
								if (!orderValue) {
									printf("get name faild !\n");
									printf("Error before: [%s]\n", cJSON_GetErrorPtr());
									USART3_RX_STA = 0;
									cJSON_Delete(root);
									continue;
								}	
								//strcpy(GoodsLocation.height,orderValue->valuestring);
								length =  atof(orderValue->valuestring)/1000;
								sprintf(GoodsLocation.height,"%f",length);

								orderValue = cJSON_GetObjectItem(data, "depth");
								if (!orderValue) {
									printf("get name faild !\n");
									printf("Error before: [%s]\n", cJSON_GetErrorPtr());
									USART3_RX_STA = 0;
									cJSON_Delete(root);
									continue;
								}	
								//strcpy(GoodsLocation.depth,orderValue->valuestring);
								length =  atof(orderValue->valuestring)/1000;
								sprintf(GoodsLocation.depth,"%f",length);
								
								printf("\r\nside:%s,distance:%s,height:%s,depth:%s",GoodsLocation.side, GoodsLocation.distance, GoodsLocation.height, GoodsLocation.depth);
	
							}

						USART3_RX_STA = 0;
						cJSON_Delete(root);
						printf("\r\n master get businessType:%d from server",businessType);
						return ;  //get goalType.exit
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

		
		delay_ms(200);
		
		printfNUM++;
		if (printfNUM ==10)
			{
				printf("\r\n waiting  order from  server!!!");
				printf("\r\need businessType:%d",goalType);
				
				
				printfNUM =0;
			}

		

		netTime++;
		if (netTime == 100)
			{
				checkORReconnect();
				netTime = 0;
			}
		
	}


}


/**************************************************************************
函数功能：给服务端发主控的角�?
入口参数：无
返回  值：�?
**************************************************************************/
void sendMasterID2S()
{
	cJSON *root, *data;  // 

	int num = 0;
	int numS = 0;

	char* strSend;
	char send[200];


	printf("\r\n start sendMasterID2S");
	
	//  给服务器�?
	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0000");
	cJSON_AddItemToObject(root, "data", data=cJSON_CreateObject());
	cJSON_AddStringToObject(data,"ID", MASTER_ID);

	strSend=cJSON_PrintUnformatted(root); 
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

	// 加协议头�?
	memset(send, 0, sizeof(send));
	send[0] = '#';
	send[1] = '!';
	strncpy(send+2, strSend, num); 
	send[num+2] = '&';
	send[num+3] = '\n';

	WIFISend(send);

	
	aiwacFree(strSend);

	printf("\r\nstrSend:%s",send);

	printf("\r\n sendMasterID2S  OK");
	

}




/**************************************************************************
函数功能：wifi模块数据发送函数，封装了下发送的逻辑
入口参数：无
返回  值：�?
**************************************************************************/
void WIFISend(char* MS)
{
	checkORReconnect();  // 检查连接是否在线

	atk_8266_quit_trans();
	atk_8266_send_cmd("AT+CIPSEND","OK",20);		 //开始透传 	   

	u3_printf("%s",MS);
	
}



/**************************************************************************
函数功能：主控取货的逻辑
入口参数：数据指�?
返回  值：�?
**************************************************************************/
void  AIWAC_MasterGetGoods(void)
{
	while(1)
	{   
		initSysValue();				// 初始化系统的全局变量
		controlCarToInitSpace();	// 回到复位�?
		waitingSAskState();			// 等待服务端查询状态，并反�?
		waitingSSendLocation();		// 获取位置，取�?
		DropGoods();				// 放货
		DropPan();					// 放盘子，并复�?
		delay_ms(100);
		printf("\r\n finish one time !!");
	}

}


/**************************************************************************
函数功能：查询小�?取货单元的状态，并等待回�?
入口参数：无
返回  值：�?
**************************************************************************/
void askState2other(void )
{

	u16 jsonSize;
	cJSON *root;
	char *strJson;
	u8 strSend[100];

	while (1)
	{
		// 查询car1
		memset(strSend, 0, sizeof(strSend));
		strSend[0] = '#';
		strSend[1] = '!';


		root=cJSON_CreateObject();

		cJSON_AddStringToObject(root,"businessType", "0007");

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
		usart2_sendString(strSend,7 + jsonSize);
		aiwacFree(strJson);

		delay_ms(200);
		if(SystemState.car1State>0)
		{
			break;
		}

		printf("\r\nwaiting car1  feedback State!!");
		
	}
	


	while (1)
	{

		// 查询car2
		memset(strSend, 0, sizeof(strSend));
		strSend[0] = '#';
		strSend[1] = '!';

		root=cJSON_CreateObject();

		cJSON_AddStringToObject(root,"businessType", "0008");

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
		aiwacFree(strJson);
		
		delay_ms(200);
		if(SystemState.car2State>0)
		{
			break;
		}

		printf("\r\nwaiting car2  feedback State!!");
		
	}


	while (1)
	{
		// 查询取货单元
		memset(strSend, 0, sizeof(strSend));
		strSend[0] = '#';
		strSend[1] = '!';

		root=cJSON_CreateObject();

		cJSON_AddStringToObject(root,"businessType", "0013");

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
		uart4_sendString(strSend,7 + jsonSize);
		aiwacFree(strJson);

		delay_ms(200);
		if(SystemState.goodsGetterState>0)
		{
			break;
		}

		printf("\r\nwaiting getter feedback State!!");
		
	}
	
	printf("\r\n askState2other:master has gotten other state!!");


}



/**************************************************************************
函数功能:检查系统的 状态，主控需要检查当前的位置是否在复位点
入口参数：无
返回  值：�?
**************************************************************************/
void   checkSysState(void)
{	
	int ret = 1; // 状态标�?	1:ok  0:error
	char errorDesc[500]; 

	cJSON *root, *data;  // 

	int num = 0;
	int numS = 0;

	char* strSend;
	char send[200];


	
	
	printf("\r\nwaiting for feedbacking state");
	delay_ms(1000);
	
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
		strcat(errorDesc,SystemState.goodsGetterEorror);
	}


// 为了测试
/*
	// 未到中间的位�?
	// 注意：后面需要调�?
	if ( (myabs_double(Car1_FDistance-MIDDLE_SPACE)>0.03) || (myabs_double(Car1_FDistance-MIDDLE_SPACE)>0.03))
	{
		ret = 0;
		printf("\r\n the state of location is bad,Cars are not good space!!");
		delay_ms(10);
		strcat(errorDesc,"MIDDLE_SPACE:");
		strcat(errorDesc,"the state of location is bad,Cars are not good space");
	}

*/




	//  给服务器发状�?
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

	strSend=cJSON_PrintUnformatted(root); 
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

	// 加协议头�?
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
		
		delay_ms(1000);
		printf("\r\nstate error. errorDesc:%s",errorDesc);

	}


	printf("\r\ncheckSysState ok");

}


/**************************************************************************
函数功能：step1:等待  服务�? 查询复位情况并反馈复位情�?
入口参数：无
返回  值：�?
**************************************************************************/
void waitingSAskState(void)
{
	printf("\r\n enter waitingSAskState");
	
	parseOrderFromS(1);  // 等待 服务�?发起 状态查�?
	feedbackGotOrder(1); //向服务端反馈收到额指�?
	askState2other();	 // 发起两小车和取货单元的状态查�?并等待反�?
	checkSysState();	// 检查状态，反馈给服务端
}

/**************************************************************************
函数功能：step2:等待  服务�? 下发订单坐标并反馈取到货
入口参数：无
返回  值：�?
**************************************************************************/
void waitingSSendLocation(void)
{
	printf("\r\n enter waitingSSendLocation");
	parseOrderFromS(3);  			// 等待 android端向主控提供需要取的货物的位置信息�?
	feedbackGotOrder(3); 			// 向服务端反馈收到额指�?
	feedbackStartGetGoods(); 		// 通知服务端开始取�?
	controlCarToGoodsSpace();		// 控制小车运动到货物点
	notifyGoodsGetterLocation();	// 给取货单�? 商品的位置和深度
	waitingGetterGotGoods();		// 等待取货单元反馈取到�?

	feedbackGotGoodsResult();		// 反馈已经取到货物
} 


/**************************************************************************
函数功能：step3:进入放货逻辑并反馈已经放�?
入口参数：无
返回  值：�?
**************************************************************************/
void DropGoods(void)
{
	printf("\r\n enter DropGoods");
	controlCarToGate();				// 控制小车�?放货�?
	notifyGoodsGetterLoseGoods();	// 通知取货单元放货
	waitingGetterLoseGoods();		// 等待取货单元放货
	feedbackLoseGoodsResult();		// 给服务端反馈 放货情况
}



/**************************************************************************
函数功能：step4:进入复位丢盘逻辑并反馈已经复�?
入口参数：无
返回  值：�?
**************************************************************************/
void DropPan(void)
{
	printf("\r\n enter DropPan");

	controlCarToDropPan();		// 控制小车到丢盘子的地�?
	notifyGoodsGetterDropPan();	// 通知取货单元丢盘�?
	waitingGetterLosePan();	// 等待取货单元丢盘�?
	controlCarToInitSpace();	// 控制小车到复位点
	feedbackGoInit();			// 反馈已经复位
}



/**************************************************************************
函数功能：解析来自小�?的数据，串口2
入口参数：无
返回  值：�?
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

	//printf("\r\n car1 businessType:%s",orderValue->valuestring);


	// 反馈小车的状�?
	if (strcmp(orderValue->valuestring, "0007")==0)  
		{
			orderValue = cJSON_GetObjectItem(root, "errorCode");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			SystemState.car1State = orderValue->valueint;


			orderValue = cJSON_GetObjectItem(root, "errorDesc");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			strcpy(SystemState.car1Error,orderValue->valuestring );

			goto end;

		}

	// 反馈小车的方向距�?
	if (strcmp(orderValue->valuestring, "0011")==0)  
		{
			//printf("\r\n0011");
			orderValue = cJSON_GetObjectItem(root, "Co");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car1_CorrectState = orderValue->valueint;

			orderValue = cJSON_GetObjectItem(root, "FD");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car1_FDistance = orderValue->valuedouble;

			orderValue = cJSON_GetObjectItem(root, "BD");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car1_BDistance = orderValue->valuedouble;

			orderValue = cJSON_GetObjectItem(root, "mo");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car1_moveState = orderValue->valueint;


			goto end;

		}



end :
	cJSON_Delete(root);

	USART2_Car1_jsonParseBuF[0] = '-' ;
}


/**************************************************************************
函数功能：解析来自小�?的数据，串口5
入口参数：无
返回  值：�?
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
	//printf("\r\n car2 businessType:%s",orderValue->valuestring);


	// 反馈小车的状�?
	if (strcmp(orderValue->valuestring, "0008")==0)  
		{
			orderValue = cJSON_GetObjectItem(root, "errorCode");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			SystemState.car2State = orderValue->valueint;


			orderValue = cJSON_GetObjectItem(root, "errorDesc");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			strcpy(SystemState.car2Error,orderValue->valuestring );

			goto end;

		}

	// 反馈小车的方向距�?
	if (strcmp(orderValue->valuestring, "0012")==0)  
		{//printf("\r\n0012");
			orderValue = cJSON_GetObjectItem(root, "Co");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car2_CorrectState = orderValue->valueint;

			orderValue = cJSON_GetObjectItem(root, "FD");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car2_FDistance = orderValue->valuedouble;

			orderValue = cJSON_GetObjectItem(root, "BD");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car2_BDistance = orderValue->valuedouble;

			orderValue = cJSON_GetObjectItem(root, "mo");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			Car2_moveState = orderValue->valueint;


			goto end;

		}



end :
	cJSON_Delete(root);
	USART5_Car2_jsonParseBuF[0] = '-';

}

/**************************************************************************
函数功能：解析来自取货单元的数据，串�?
入口参数：无
返回  值：�?
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



	// 取货单元给主控反�?状�?
	if (strcmp(orderValue->valuestring, "0013")==0)  
		{
			orderValue = cJSON_GetObjectItem(root, "errorCode");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			SystemState.goodsGetterState = orderValue->valueint;

			orderValue = cJSON_GetObjectItem(root, "errorDesc");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			strcpy(SystemState.goodsGetterEorror,orderValue->valuestring );

			goto end;



		}

	// 取货单元给主控反�?取货情况
	if (strcmp(orderValue->valuestring, "0015")==0)  
		{
			orderValue = cJSON_GetObjectItem(root, "Result");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			GotGoodsResult = orderValue->valueint;

			goto end;

		}

	// 取货单元给主控反�?卸货情况
	if (strcmp(orderValue->valuestring, "0017")==0)  
		{
			orderValue = cJSON_GetObjectItem(root, "Result");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			LoseGoodsResult = orderValue->valueint;

			goto end;

		}

		// 取货单元给主控反�?丢盘子情�?
	if (strcmp(orderValue->valuestring, "0019")==0)  
		{
			orderValue = cJSON_GetObjectItem(root, "Result");  
			if (!orderValue) {
					//printf("get name faild !\n");
					//printf("Error before: [%s]\n", cJSON_GetErrorPtr());
					goto end;
				}
			LosePanResult = orderValue->valueint;

			goto end;

		}


end :
	cJSON_Delete(root);

	USART4_Getter_jsonParseBuF[0] = '-';


}


/**************************************************************************
函数功能：绝对值函�?
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
入口参数：businessTypeGot�?收到的指�?
返回  值：�?
**************************************************************************/
void feedbackGotOrder(int businessTypeGot)
{
	cJSON *root, *data;  // 
	char TypeGot[10];
	
	int num = 0;
	int numS = 0;

	char* strSend;
	char send[100];
	
	sprintf(TypeGot, "%04d", businessTypeGot); 

	//	给服务器发状�?
	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0024");
	cJSON_AddItemToObject(root, "data", data=cJSON_CreateObject());

	
	cJSON_AddStringToObject(data,"businessTypeGot", TypeGot);



	strSend=cJSON_PrintUnformatted(root); 
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

	// 加协议头�?
	memset(send, 0, sizeof(send));
	send[0] = '#';
	send[1] = '!';
	strncpy(send+2, strSend, num); 
	send[num+2] = '&';
	send[num+3] = '\n';

	WIFISend(send);

	//printf("\r\n feedback to server ,strSend:%s  LEN:%\d",strSend,strlen(strSend));
	aiwacFree(strSend);
	delay_ms(150);
	printf("\r\nfeedback to server ,strSend:%s",send);


}


/**************************************************************************
函数功能：向服务端反馈开始取�?
入口参数：无
返回  值：�?
**************************************************************************/
void feedbackStartGetGoods(void)
{

	cJSON *root, *data;  // 

	int num = 0;
	int numS = 0;

	char* strSend;
	char send[200];

	//	给服务器发状�?
	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0004");
	cJSON_AddItemToObject(root, "data", data=cJSON_CreateObject());
	cJSON_AddStringToObject(data,"status", "1");
	cJSON_AddStringToObject(data,"errorDesc", "fff");

	strSend=cJSON_PrintUnformatted(root); 
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

	// 加协议头�?
	memset(send, 0, sizeof(send));
	send[0] = '#';
	send[1] = '!';
	strncpy(send+2, strSend, num); 
	send[num+2] = '&';
	send[num+3] = '\n';

	WIFISend(send);

	printf("\r\n feedback to server ,strSend:%s  LEN:%\d",strSend,strlen(strSend));
	aiwacFree(strSend);

	printf("\r\n feedbackStartGetGoods");


}


/**************************************************************************
函数功能：控制小车到货物�?
入口参数：无
返回  值：�?
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
	printf("\r\n controlCarToGoodsSpace");
	

}


/**************************************************************************
函数功能：通知取货单元，商�?高度和深�?
入口参数：无
返回  值：�?
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
	uart4_sendString(strSend,7 + jsonSize);
	aiwacFree(strJson);

	printf("\r\n notifyGoodsGetterLocation:%s",strSend);


}


/**************************************************************************
函数功能：等待取货单元反馈取到货
入口参数：无
返回  值：�?
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
				printf("\r\n wating to geting  Goods");
			}
		
	}
	delay_ms(100);
	
	printf("\r\n waitingGetterGotGoods,result:%d",GotGoodsResult);

}


/**************************************************************************
函数功能：向服务端反馈取货情�?
入口参数：无
返回  值：�?
**************************************************************************/
void feedbackGotGoodsResult(void)
{

	cJSON *root, *data;  // 

	int num = 0;
	int numS = 0;

	char* strSend;
	char send[200];

	//	给服务器发状�?
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

	strSend=cJSON_PrintUnformatted(root); 
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

	// 加协议头�?
	memset(send, 0, sizeof(send));
	send[0] = '#';
	send[1] = '!';
	strncpy(send+2, strSend, num); 
	send[num+2] = '&';
	send[num+3] = '\n';

	WIFISend(send);

	printf("\r\n feedback to server ,strSend:%s  LEN:%\d",strSend,strlen(strSend));
	aiwacFree(strSend);

	printf("\r\n feedbackGotGoodsResult");
	

}



/**************************************************************************
函数功能：控制小车到放货�?
入口参数：无
返回  值：�?
**************************************************************************/
void controlCarToGate(void)
{
	int goalSide = 0;
	goalSide = 1;  // 出货�?在A  -> 1



	printf("\r\n controlCarToGate:	goalSide:%d, LocationNow:%d",goalSide, LocationNow);
	
	goToEverywhere(goalSide, LocationNow, DROP_GOODS_SPACE);
	
	LocationNow = goalSide;
	printf("\r\n controlCarToGate");
		


}


/**************************************************************************
函数功能：通知取货单元放货
入口参数：无
返回  值：�?
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
	cJSON_AddStringToObject(root,"businessType", "0016");

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
	uart4_sendString(strSend,7 + jsonSize);
	aiwacFree(strJson);

	printf("\r\n notifyGoodsGetterLoseGoods:%s",strSend);


}

/**************************************************************************
函数功能：等待取货单元反馈放�?
入口参数：无
返回  值：�?
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
				printf("\r\n wating losing Goods");
			}
	}
	delay_ms(100);
	
	printf("\r\n waitingGetterLoseGoods,result:%d",LoseGoodsResult);

}




/**************************************************************************
函数功能：向服务端反馈放货情�?
入口参数：无
返回  值：�?
**************************************************************************/
void feedbackLoseGoodsResult(void)
{

	cJSON *root, *data;  // 

	int num = 0;
	int numS = 0;

	char* strSend;
	char send[200];

	//	给服务器发状�?
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

	strSend=cJSON_PrintUnformatted(root); 
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

	// 加协议头�?
	memset(send, 0, sizeof(send));
	send[0] = '#';
	send[1] = '!';
	strncpy(send+2, strSend, num); 
	send[num+2] = '&';
	send[num+3] = '\n';

	WIFISend(send);

	printf("\r\n feedback to server ,strSend:%s  LEN:%\d",strSend,strlen(strSend));
	aiwacFree(strSend);
	printf("\r\n feedbackLoseGoodsResult");
	

}


/**************************************************************************
函数功能：控制小车到丢盘子的地方
入口参数：无
返回  值：�?
**************************************************************************/
void controlCarToDropPan(void)
{
	int goalSide = 0;
	goalSide = 1;  // 丢盘�?在A  -> 1

	printf("\r\n controlCarToDropPan:	goalSide:%d, LocationNow:%d",goalSide, LocationNow);
	
	goToEverywhere(goalSide, LocationNow, DROP_PAN_SPACE);
	
	LocationNow = goalSide;
	printf("\r\n controlCarToDropPan");
		

}


/**************************************************************************
函数功能：通知取货单元，丢盘子
入口参数：无
返回  值：�?
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
	uart4_sendString(strSend,7 + jsonSize);
	aiwacFree(strJson);

	printf("\r\n notifyGoodsGetterDropPan:%s",strSend);


}

/**************************************************************************
函数功能：等待取货单元丢盘子
入口参数：无
返回  值：�?
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
				printf("\r\n wating losing Pan");
			}
	}
	delay_ms(100);
	
	printf("\r\n waitingGetterLosePan,result:%d",LosePanResult);

}



/**************************************************************************
函数功能：控制小车到复位�?
入口参数：无
返回  值：�?
**************************************************************************/
void controlCarToInitSpace(void)
{
	int goalSide = 0;
	goalSide = 2;  // 复位�?在B  -> 2



	printf("\r\n controlCarToInitSpace:	goalSide:%d, LocationNow:%d",goalSide, LocationNow);
	
	goToEverywhere(goalSide, LocationNow, CAR_INIT_SPACE);
	
	LocationNow = goalSide;
	
	printf("\r\n controlCarToInitSpace OK");


}

/**************************************************************************
函数功能：向服务端反馈复位情�?
入口参数：无
返回  值：�?
**************************************************************************/
void feedbackGoInit(void)
{

	cJSON *root, *data;  // 

	int num = 0;
	int numS = 0;

	char* strSend;
	char send[200];

	//	给服务器发状�?
	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0002");
	cJSON_AddItemToObject(root, "data", data=cJSON_CreateObject());
	cJSON_AddStringToObject(data,"status", "4");
	cJSON_AddStringToObject(data,"errorDesc", "fff");

	strSend=cJSON_PrintUnformatted(root); 
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

	// 加协议头�?
	memset(send, 0, sizeof(send));
	send[0] = '#';
	send[1] = '!';
	strncpy(send+2, strSend, num); 
	send[num+2] = '&';
	send[num+3] = '\n';

	WIFISend(send);

	printf("\r\n feedback to server ,strSend:%s  LEN:%\d",strSend,strlen(strSend));
	aiwacFree(strSend);
	printf("\r\n ffeedbackGoInit");

}


/**************************************************************************
函数功能�?指定方向，运动到某方向剩余的距离�?
入口参数�? direction：方�?   		needDistance：剩余的距离
返回  值：		�?
**************************************************************************/
void goToLocation(int direction,double needDistance)
{

	while (1)
	{	
		if( (Car1_CorrectState > -1) && (Car2_CorrectState > -1)) // 还未完全收到两小车的  数据
		{
			
			break;
		}

		printf("\r\n waiting for data from cars");	
		printf("\r\nCar1_CorrectState:%d,Car2_CorrectState:%d",Car1_CorrectState,Car2_CorrectState);
		delay_ms(100); 
	}

			
	//起步阶段，需要两车平行起�?
	goStartTogether(direction);

	// 运动到某方向的，指定地点
	goGoalPosition( direction,needDistance);

}



/**************************************************************************
函数功能�?两车起步的逻辑，在指定方向让后面的�?移动到前面，然后等矫�?
入口参数�?int direction  方向 
返回  值：		�?
**************************************************************************/
void goStartTogether(int direction)
{
	double goalLocation = 0 ;
	double TogetherGap = 0.02;

	while(( Car1_moveState > 1 )|| (Car2_moveState > 1) ) //  当前�? 小车�? 转弯
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


	

	//等校�?
	while (1) 
		{
			delay_ms(50);
			
			if (  (Car1_CorrectState  == 1) && ( Car2_CorrectState == 1) )//姿态校�? ok
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

				
			if (myabs_double(Car1_FDistance- goalLocation) <= TogetherGap)  // �?是标准位�?
				{
					printf("\r\n goStartTogether:study from car1 ");
					
					while (1)
					{
						delay_ms(80);
	

						if (myabs_double(Car2_FDistance- goalLocation) <= TogetherGap) // �? ok
							{
								printf("\r\n goStartTogether:CorrectState   ok");
								AiwacMasterSendOrderCar2(CAR_STOP , STATE_STOP) ;
								break;
							}

						
						if( (Car2_FDistance)< goalLocation - TogetherGap) //走超�?
							{
								AiwacMasterSendOrderCar2(-MIN_SPEED , STATE_STRAIGHT) ;
								printf("\r\n goStartTogether:over");

							}
						else if ((Car2_FDistance)> 4*TogetherGap+ goalLocation) //还较�?
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

						if (myabs_double(Car1_FDistance- goalLocation) <= TogetherGap) // �? ok
							{
								printf("\r\n goStartTogether:CorrectState   ok");
								AiwacMasterSendOrderCar1(CAR_STOP , STATE_STOP) ;
								break;
							}

						
						if( (Car1_FDistance)< goalLocation - TogetherGap) //走超�?
							{
								AiwacMasterSendOrderCar1(-MIN_SPEED , STATE_STRAIGHT) ;
								printf("\r\n goStartTogether:over");

							}
						else if ((Car1_FDistance)> 4*TogetherGap+ goalLocation) //还较�?
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

					
			if (myabs_double(Car1_BDistance- goalLocation) <= TogetherGap)  // �?是标准位�?
			{
				printf("\r\n goStartTogether:study from car1 ");
				
				while (1)
				{
					delay_ms(80);


					if (myabs_double(Car2_BDistance- goalLocation) <= TogetherGap) // �? ok
						{
							printf("\r\n goStartTogether:CorrectState   ok");
							AiwacMasterSendOrderCar2(CAR_STOP , STATE_STOP) ;
							break;
						}

					
					if( (Car2_BDistance)< goalLocation - TogetherGap) //走超�?
						{
							AiwacMasterSendOrderCar2(MIN_SPEED , STATE_STRAIGHT) ;
							printf("\r\n goStartTogether:over");

						}
					else if ((Car2_BDistance)> 4*TogetherGap+ goalLocation) //还较�?
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
					if (myabs_double(Car1_BDistance- goalLocation) <= TogetherGap) // �? ok
						{
							printf("\r\n goStartTogether:CorrectState   ok");
							AiwacMasterSendOrderCar1(CAR_STOP , STATE_STOP) ;
							break;
						}

					
					if( (Car1_BDistance)< goalLocation - TogetherGap) //走超�?
						{
							AiwacMasterSendOrderCar1(MIN_SPEED , STATE_STRAIGHT) ;
							printf("\r\n goStartTogether:over");

						}
					else if ((Car1_BDistance)> 4*TogetherGap+ goalLocation) //还较�?
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




	//等校�?
	while (1) 
		{
			delay_ms(50);
			
			if (  (Car1_CorrectState  == 1) && ( Car2_CorrectState == 1) )//姿态校�? ok
			{
				printf("\r\n step3  goStartTogether correction ok ");
				break;
			}

		}


}





/**************************************************************************
函数功能�?两车已经平行，在指定方向运动到目标地�?
入口参数�?int direction  方向 
返回  值：		�?
**************************************************************************/
void goGoalPosition(int direction,double NeedDistance)
{
	double goalGAP = 0.015;   //m
	double iniTDistance = 0; // 起步距离，用�?渐进起步
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
	

	//等校�?
	while (1) 
		{
			delay_ms(80);
			
			if (  (Car1_CorrectState  == 1) && ( Car2_CorrectState == 1) )//姿态校�? ok
			{
				printf("\r\n step1  goGoalPosition correction ok ");
				break;
			}

		}


	
	// 运动到目标地�?
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


							
						
							// �? 的情�?
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


							
						
						// �? 的情�?
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
							if( myabs_double(Car1_FDistance - Car2_FDistance ) < goalGAP*4)  //  两车�?前进 距离ok
							{
								// 下发  继续 默认前进 
								AiwacMasterSendOrderCar1(designFSpeed2((Car1_FDistance+Car2_FDistance)/2, needDistance,iniTDistance) , STATE_STRAIGHT) ;
								AiwacMasterSendOrderCar2(designFSpeed2((Car1_FDistance+Car2_FDistance)/2, needDistance,iniTDistance) , STATE_STRAIGHT) ;
								printf("\r\ngo on straight");
							}
							else // 两车�? 前进 距离  no
							{
								if (Car1_FDistance - Car2_FDistance >0)  // 1车在�?
									{
										// 发�?2车默认速度�?�?比默认快�?
										AiwacMasterSendOrderCar1(designFSpeed2((Car1_FDistance+Car2_FDistance)/2, needDistance,iniTDistance)  +MIN_SPEED*2, STATE_STRAIGHT) ;
										AiwacMasterSendOrderCar2((designFSpeed2((Car1_FDistance+Car2_FDistance)/2, needDistance,iniTDistance) ), STATE_STRAIGHT) ;
										printf("\r\n car1 needs to go fast");
									}
								else
									{
										// 发�?1车默认速度�?�?比默认快�?
										AiwacMasterSendOrderCar1(designFSpeed2((Car1_FDistance+Car2_FDistance)/2, needDistance,iniTDistance) , STATE_STRAIGHT) ;
										AiwacMasterSendOrderCar2(designFSpeed2((Car1_FDistance+Car2_FDistance)/2, needDistance,iniTDistance)+ MIN_SPEED*2 , STATE_STRAIGHT) ;
										printf("\r\n car2 needs to go fast");
									}
								
							}

						}
				}

	}
	else		//后面的�?
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


						
					
						// �? 的情�?
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


						
					
					// �? 的情�?
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
						if( myabs_double(Car1_BDistance - Car2_BDistance ) < goalGAP*4)  //  两车�?前进 距离ok
						{
							// 下发  继续 默认前进 
							AiwacMasterSendOrderCar1(-(designFSpeed2((Car1_BDistance+Car2_BDistance)/2, needDistance,iniTDistance)) , STATE_STRAIGHT) ;
							AiwacMasterSendOrderCar2(-(designFSpeed2((Car1_BDistance+Car2_BDistance)/2, needDistance,iniTDistance)) , STATE_STRAIGHT) ;
							printf("\r\ngo on straight");
						}
						else // 两车�? 前进 距离  no
						{
							if (Car1_BDistance - Car2_BDistance >0)  // 1车在�?
								{
									// 发�?2车默认速度�?�?比默认快�?
									AiwacMasterSendOrderCar1(-(designFSpeed2((Car1_FDistance+Car2_FDistance)/2, needDistance,iniTDistance)  +MIN_SPEED*2), STATE_STRAIGHT) ;
									AiwacMasterSendOrderCar2(-(designFSpeed2((Car1_FDistance+Car2_FDistance)/2, needDistance,iniTDistance) ), STATE_STRAIGHT) ;
									printf("\r\n car1 needs to go fast");
								}
							else
								{
									// 发�?1车默认速度�?�?比默认快�?
									AiwacMasterSendOrderCar1(-(designFSpeed2((Car1_FDistance+Car2_FDistance)/2, needDistance,iniTDistance) ), STATE_STRAIGHT) ;
									AiwacMasterSendOrderCar2(-(designFSpeed2((Car1_FDistance+Car2_FDistance)/2, needDistance,iniTDistance)+ MIN_SPEED*2) , STATE_STRAIGHT) ;
									printf("\r\n car2 needs to go fast");
								}
							
						}

					}
			}

		}





	AiwacMasterSendOrderCar1(CAR_STOP , STATE_STOP) ;
	AiwacMasterSendOrderCar2(CAR_STOP , STATE_STOP) ;
	delay_ms(50);

		//等校�?
	while (1) 
		{
			delay_ms(50);
			
			if (  (Car1_CorrectState  == 1) && ( Car2_CorrectState == 1) )//姿态校�? ok
			{
				printf("\r\n step3  goGoalPosition correction ok ");
				break;
			}

		}


}



void sendTuringOrder(int Left_or_Right)
{

	//	转弯�? 方向 要看 �?超市哪边
	AiwacMasterSendOrderCar1(CAR_STOP , Left_or_Right) ;
	AiwacMasterSendOrderCar2(CAR_STOP , Left_or_Right) ;
	delay_ms(120);

	//若未进入转弯
	while ((Car2_moveState <2 ) || (Car1_moveState <2 ) )  //有车未转�?
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

	
	// 检�? 是否结束
	while((( Car1_moveState > 1 )|| (Car2_moveState > 1) ))
		{

			printf("\r\nwaiting for turing,  Car1_moveState :%d,  Car2_moveState:%d ",Car1_moveState ,Car2_moveState );
			delay_ms(10);
		}

	
	printf("\r\nturing  over");
}




/**************************************************************************
函数功能�?根据前方距离 定小车前进速度
入口参数�? 前方
返回  值：		前方速度
**************************************************************************/
double  designFSpeed2(double FD, double FD_care,double iniTDistance)
{
	double FSpeed = 30;		// 低速的速度 mm

	double FDSMax = FD_MAX_SPEED;  // 规定的最�? 前方速度  mm

	double startSpeed = 0;
	FD_care = FD_care + 0.10 ;	// 前方警戒距离，需�? 低速前�?



	if ((iniTDistance >=FD-0.05) || (iniTDistance -FD)*1000 <150)
		{
			startSpeed = (iniTDistance -FD)*700*2+FSpeed;
	
		}

	
	if (FD>FD_care)  // 离危险距离较�?
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
函数功能�?给小�?�?速度 �?小车的运动状态指�?
入口参数�? X_V  : X轴的速度,前进速度			     moveState：小车的运动状态指�?
返回  值：		�?
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
	aiwacFree(strJson);


}



/**************************************************************************
函数功能�?给小�?�?速度 �?小车的运动状态指�?
入口参数�? X_V  : X轴的速度,前进速度			     moveState：小车的运动状态指�?
返回  值：		�?
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
	aiwacFree(strJson);


}



/**************************************************************************
函数功能�?给定当前位置 �?  目标位置 控制小车运动
入口参数：goalSide：目标边   ,		nowSide：当前边   	goDistance；按图的距离
返回  值：		�?
**************************************************************************/
void goToEverywhere(int goalSide,int nowSide, double goDistance)
{
	//按分区进�? 主控逻辑
	if (strcmp("Area1", MASTER_ID)  == 0)
	{




		// 当前在A 
		
		if ((nowSide == 1))
		{
			// 在A,去A
			if (goalSide == 1)		
			{
				printf("\r\n A->A");
				if (Car1_FDistance >= goDistance)
				{
					printf("\r\n (Car1_FDistance >= goDistance)");
					goToLocation(FRONT_DIRECTION, goDistance);
				}
				else
				{
					printf("\r\n (Car1_FDistance < goDistance)");
					goToLocation(BACK_DIRECTION, A_HALF_LEN - goDistance);
				}

				return;
			}
				
			// 在A,去B
			if (goalSide == 2)		
			{
				printf("\r\n A->B");
				goToLocation(BACK_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_LEFT);
				goToLocation(BACK_DIRECTION, B_LEN - goDistance);

				return;
			}
			
			// 在A,去c
			if (goalSide == 3)	
			{
				printf("\r\n A->C");
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
				printf("\r\n B->A");
				goToLocation(FRONT_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_RIGHT);
				goToLocation(FRONT_DIRECTION, goDistance);
				return;
			
			}
			
			// 在B,去B
			if (goalSide == 2) 		
			{
				printf("\r\n B->B");
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
				printf("\r\n B->C");
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
				printf("\r\n C->A");
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
				printf("\r\n C->B");
				goToLocation(FRONT_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_RIGHT);
				goToLocation(FRONT_DIRECTION, goDistance);

				return;
			}

			// 在C,去C
			if (goalSide == 3) 		
			{
				printf("\r\n C->C");
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
				goToLocation(BACK_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_LEFT);
				goToLocation(BACK_DIRECTION, TURING_DISTANCE);
				sendTuringOrder(STATE_TURN_LEFT);
				goToLocation(BACK_DIRECTION, goDistance);
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


void test11(void)
{
	u8 getMS[300];
	cJSON *root, *orderValue,*data;  // 
	int rlen = 0;

	int businessType =999;
	int goalType =3;


	
	char testChar[300] = "#!{	\"businessType\": \"0003\", 	\"data\": {               		\"side\": \"A\",       		\"distance\": \"1200\", 		\"height\": \"1000\", 		\"depth\": \"100\"    }}&";
	rlen = strlen(testChar);

	if (	(testChar[0] == '#') 
				&&	(testChar[1] == '!')
				&&	(testChar[rlen-1] == '&')
				)
			{

				strncpy(getMS, testChar+2, rlen-3); 
				printf("\r\n getMs:%s",getMS);
				
				root = cJSON_Parse(getMS);
				if (!root) 
				{
					printf("root :Error before: [%s]\n",cJSON_GetErrorPtr());
					USART3_RX_STA = 0;
					return;
				}


				orderValue = cJSON_GetObjectItem(root, "businessType");  //  ×?D￡�??é??
				if (!orderValue) {
					printf("get name faild !\n");
					printf("businessType :Error before: [%s]\n", cJSON_GetErrorPtr());
					USART3_RX_STA = 0;
					cJSON_Delete(root);
					return;
				}

				
				businessType = atoi(orderValue->valuestring);
				if (businessType == goalType)  //进行目标消息类型的处�?
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
									printf("data:Error before: [%s]\n", cJSON_GetErrorPtr());
									USART3_RX_STA = 0;
									cJSON_Delete(root);
									return;
								}

								orderValue = cJSON_GetObjectItem(data, "side");
								if (!orderValue) {
									printf("get name faild !\n");
									printf("side:Error before: [%s]\n", cJSON_GetErrorPtr());
									USART3_RX_STA = 0;
									cJSON_Delete(root);
									return;
								}	
								strcpy(GoodsLocation.side,orderValue->valuestring);

								orderValue = cJSON_GetObjectItem(data, "distance");
								if (!orderValue) {
									printf("get name faild !\n");
									printf("distance:Error before: [%s]\n", cJSON_GetErrorPtr());
									USART3_RX_STA = 0;
									cJSON_Delete(root);
									return;
								}	
								strcpy(GoodsLocation.distance,orderValue->valuestring);


								orderValue = cJSON_GetObjectItem(data, "height");
								if (!orderValue) {
									printf("get name faild !\n");
									printf("height:Error before: [%s]\n", cJSON_GetErrorPtr());
									USART3_RX_STA = 0;
									cJSON_Delete(root);
									return;
								}	
								strcpy(GoodsLocation.height,orderValue->valuestring);

								orderValue = cJSON_GetObjectItem(data, "depth");
								if (!orderValue) {
									printf("get name faild !\n");
									printf("depth:Error before: [%s]\n", cJSON_GetErrorPtr());
									USART3_RX_STA = 0;
									cJSON_Delete(root);
									return;
								}	
								strcpy(GoodsLocation.depth,orderValue->valuestring);
								printf("\r\nside:%s,distance:%s,height:%s,depth:%s",GoodsLocation.side, GoodsLocation.distance, GoodsLocation.height, GoodsLocation.depth);

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
						return;
					}
				
			}	
			else
			{
				printf("\r\n the ms   error!!!");
				printf("\r\n USART3_RX_BUF:%s!!!",testChar);
				
			
			}


}


void checkORReconnect(void )
{	u8 constate;
	constate=atk_8266_consta_check();//得到连接状态
	
	if (constate != '+')
	{
		printf("\r\n rebuilt the net link");
		
		wifi_Init();				// wifi模块初始化，完成连路由器，连服务端逻
		sendMasterID2S();			//给服务端发送主控ID
		atk_8266_at_response(1);

	}
	else
	{
		atk_8266_at_response(1);
		printf("\r\n  the net link  ok\r\n");
	}
	
}
