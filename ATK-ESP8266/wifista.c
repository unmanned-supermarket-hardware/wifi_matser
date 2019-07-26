#include "common.h"
#include "stdlib.h"
#include "cJSON.h"
#include "AIWAC_Supermarket.h"



/////////////////////////////////////////////////////////////////////////////////////////////////////////// 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32������
//ATK-ESP8266 WIFIģ�� WIFI STA��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2015/4/3
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
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
		
			orderValue = cJSON_GetObjectItem(root, "id");  //  ��?D���??��??
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

				
				//  ����������
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

//ATK-ESP8266 WIFI STA����
//���ڲ���TCP/UDP����
//����ֵ:0,����
//    ����,�������
u8 netpro=0;	//����ģʽ
u8 atk_8266_wifista_test(void)
{
	//u8 netpro=0;	//����ģʽ
	u8 key;
	u8 timex=0; 
	u8 ipbuf[16]; 	//IP����
	u8 *p;
	u16 t=999;		//���ٵ�һ�λ�ȡ����״̬
	u8 res=0;
	u16 rlen=0;
	u8 constate=0;	//����״̬
	p=mymalloc(SRAMIN,32);							//����32�ֽ��ڴ�
	//atk_8266_send_cmd("AT+CWMODE_DEF=1","OK",50);		//����WIFI STAģʽ
	atk_8266_send_cmd("AT+CWMODE=1","OK",50);		//����WIFI STAģʽ

	//�������ӵ���WIFI��������/���ܷ�ʽ/����,�⼸��������Ҫ�������Լ���·�������ý����޸�!! 
	sprintf((char*)p,"AT+CWJAP=\"%s\",\"%s\"",wifista_ssid,wifista_password);//�������߲���:ssid,����
	while(atk_8266_send_cmd(p,"WIFI GOT IP",300));					//����Ŀ��·����,���һ��IP
PRESTA:



	// ���������ӳɹ����߼�
	netpro = 1;

  //TCP
	
	if(1)     //TCP Client    ͸��ģʽ����
	{
		delay_ms(500);

		atk_8266_at_response(1);//WIFIģ�鷢����������,��ʱ�ϴ�������

		strcpy(ipbuf, AIWAC_IP);//  �����ip
		atk_8266_send_cmd("AT+CIPMUX=0","OK",20);   //0�������ӣ�1��������
		sprintf((char*)p,"AT+CIPSTART=\"TCP\",\"%s\",%s",ipbuf,(u8*)portnum);    //����Ŀ��TCP������

		//printf("\r\np:%s len:%d",p,strlen(p));
		while(atk_8266_send_cmd(p,"OK",200))
		{
				printf("\r\n����TCPʧ��");
		}	
		
		atk_8266_send_cmd("AT+CIPMODE=1","OK",200);      //����ģʽΪ��͸��			
	}


	LCD_Clear(WHITE);
	POINT_COLOR=RED;

	// ��Ϊ��ȡ �Լ�Ipչʾ��
	atk_8266_get_wanip(ipbuf);//������ģʽ,��ȡWAN IP
	sprintf((char*)p,"IP��ַ:%s �˿�:%s",ipbuf,(u8*)portnum);
	Show_Str(30,65,200,12,p,12,0);				//��ʾIP��ַ�Ͷ˿�	
	Show_Str(30,80,200,12,"״̬:",12,0); 		//����״̬
	Show_Str(120,80,200,12,"ģʽ:",12,0); 		//����״̬
	Show_Str(30,100,200,12,"��������:",12,0); 	//��������
	Show_Str(30,115,200,12,"��������:",12,0);	//��������

	// ��Ļչʾ����
	atk_8266_wificonf_show(30,180,"������·�������߲���Ϊ:",(u8*)wifista_ssid,(u8*)wifista_encryption,(u8*)wifista_password);
	POINT_COLOR=BLUE;
	Show_Str(120+30,80,200,12,(u8*)ATK_ESP8266_WORKMODE_TBL[netpro],12,0); 		//����״̬
	USART3_RX_STA=0;


	
	
	/*
	while(1)
	{


		//KEY0 �������� 
		key=KEY_Scan(0);
		if(key==KEY0_PRES)	
		{
		


			{
				atk_8266_quit_trans();
				atk_8266_send_cmd("AT+CIPSEND","OK",20);         //��ʼ͸��           
				sprintf((char*)p,"ATK-8266%d\r\n",t/10);//��������
				Show_Str(30+54,100,200,12,p,12,0);

				//  ��������
				u3_printf("%s",p);
				timex=100;
			}

		}else;

		t++;
		delay_ms(10);


		//���յ�һ������
		if(USART3_RX_STA&0X8000)		
		{ 
			rlen=USART3_RX_STA&0X7FFF;	//�õ����ν��յ������ݳ���
			USART3_RX_BUF[rlen]=0;		//��ӽ����� 
			printf("get data len:%d,%s",strlen(USART3_RX_BUF),USART3_RX_BUF);	//���͵�����   
			 testTS(USART3_RX_BUF);
			
			sprintf((char*)p,"�յ�%d�ֽ�,��������",rlen);//���յ����ֽ��� 
			LCD_Fill(30+54,115,239,130,WHITE);
			POINT_COLOR=BRED;
			Show_Str(30+54,115,156,12,p,12,0); 			//��ʾ���յ������ݳ���
			POINT_COLOR=BLUE;
			LCD_Fill(30,130,239,319,WHITE);
			Show_Str(30,130,180,190,USART3_RX_BUF,12,0);//��ʾ���յ�������  
			USART3_RX_STA=0;
			if(constate!='+')t=1000;		//״̬Ϊ��δ����,������������״̬
			else t=0;                   //״̬Ϊ�Ѿ�������,10����ټ��
		}  



		//����10����û���յ��κ�����,��������ǲ��ǻ�����.
		if(t ==1000)
		{
			LCD_Fill(30+54,125,239,130,WHITE);
			LCD_Fill(60,80,120,92,WHITE);
			constate=atk_8266_consta_check();//�õ�����״̬
			if(constate=='+')Show_Str(30+30,80,200,12,"���ӳɹ�",12,0);  //����״̬
			else Show_Str(30+30,80,200,12,"����ʧ��",12,0); 	 
			t=0;
		}

		atk_8266_at_response(1);  //  ��������ֱ�ӹ��������ݼ�  ǰ�� ����Ƿ�����ӵ�   ��������
	}

*/
			
	myfree(SRAMIN,p);		//�ͷ��ڴ� 
	return res;		
} 




























