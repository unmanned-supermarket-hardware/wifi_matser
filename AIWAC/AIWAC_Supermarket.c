#include "AIWAC_Supermarket.h"

#define MASTER_AREA_1 "Area1"
#define MASTER_AREA_2 "Area2"

#define MASTER_ID MASTER_AREA_1



//WIFI STAģʽ,����Ҫȥ���ӵ�·�������߲���,��������Լ���·��������,�����޸�.
const u8* wifista_ssid="AIWAC";			//·����SSID��
const u8* wifista_encryption="WPA/WPA2_PSK";	//wpa/wpa2 aes���ܷ�ʽ
const u8* wifista_password="epic2012"; 	//��������


//���Ӷ˿ں�:8086,�������޸�Ϊ�����˿�.
const u8* portnum="8899";	

int numF1 = 0;
int numF2 = 0;

int WIFIInitOK = 0;   // ���Խ����жϽ���������־

/**************************************************************************
�������ܣ���ʼ��wifiģ��
��ڲ�������
����  ֵ����
**************************************************************************/
void wifi_Init(void)
{
	atk_8266_test();		//����ATK_ESP8266
}




/**************************************************************************
�������ܣ���������wifiģ�������
��ڲ�������
����  ֵ��businessType  	�����������ط� ��
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
			rlen=USART3_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
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
				
				orderValue = cJSON_GetObjectItem(root, "id");  //  ��?D���??��??
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
�������ܣ�������˷����صĽ�ɫ
��ڲ�������
����  ֵ����
**************************************************************************/
void sendMasterID2S()
{
	cJSON *root, *data;  // 

	int num = 0;
	int numS = 0;

	char* strSend;
	char send[300];


	//  ����������
	root=cJSON_CreateObject();

	cJSON_AddStringToObject(root,"businessType", "0000");
	cJSON_AddItemToObject(root, "data", data=cJSON_CreateObject());
	cJSON_AddStringToObject(data,"ID", MASTER_ID);

	strSend=cJSON_Print(root); 
	cJSON_Delete(root); 


	// ȥ������\r\n.��׿����  һ��һ�еĽ���
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
	// ��Э��ͷ֡
	memset(send, 0, sizeof(send));
	send[0] = '#';
	send[1] = '!';
	strncpy(send+2, strSend, num+1); 
	*/

	// ��Э��ͷ֡
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
�������ܣ�wifiģ�����ݷ��ͺ�������װ���·��͵��߼�
��ڲ�������
����  ֵ����
**************************************************************************/
void WIFISend(char* MS)
{

	WIFIInitOK = 0;

	atk_8266_quit_trans();
	atk_8266_send_cmd("AT+CIPSEND","OK",20);		 //��ʼ͸�� 	   

	u3_printf("%s",MS);
	WIFIInitOK =1;
	
}



/**************************************************************************
�������ܣ�����ȡ�����߼�
��ڲ���������ָ��
����  ֵ����
**************************************************************************/
void  AIWAC_MasterGetGoods(void)
{
	while(1)
	{
		waitingSAskState();			// �ȴ�����˲�ѯ״̬��������
		waitingSSendLocation();		// ��ȡλ�ã�ȡ��
		DropGoods();				// �Ż�
		DropPan();					// �����ӣ�����λ
		delay_ms(100);
		//printf("\r\n finish one time !!");
	}


}



/**************************************************************************
�������ܣ�step1:�ȴ�  ������  ��ѯ��λ�����������λ���
��ڲ�������
����  ֵ����
**************************************************************************/
void waitingSAskState(void)
{
	while(1)
	{
		parseOrderFromS();



	}


}

/**************************************************************************
�������ܣ�step2:�ȴ�  ������  �·��������겢����ȡ����
��ڲ�������
����  ֵ����
**************************************************************************/
void waitingSSendLocation(void)
{
	


}


/**************************************************************************
�������ܣ�step3:����Ż��߼��������Ѿ��Ż�
��ڲ�������
����  ֵ����
**************************************************************************/
void DropGoods(void)
{
	


}



/**************************************************************************
�������ܣ�step4:���븴λ�����߼��������Ѿ���λ
��ڲ�������
����  ֵ����
**************************************************************************/
void DropPan(void)
{
	


}


