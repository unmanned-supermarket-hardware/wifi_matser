#include "delay.h"
#include "usart2_4_5.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"	  
#include "timer.h"
#include "AIWAC_Supermarket.h"


	
	
	unsigned char const crc8_tab[256]  = 
	{ 
		0x00,0x07,0x0E,0x09,0x1C,0x1B,0x12,0x15,0x38,0x3F,0x36,0x31,0x24,0x23,0x2A,0x2D, 
		0x70,0x77,0x7E,0x79,0x6C,0x6B,0x62,0x65,0x48,0x4F,0x46,0x41,0x54,0x53,0x5A,0x5D, 
		0xE0,0xE7,0xEE,0xE9,0xFC,0xFB,0xF2,0xF5,0xD8,0xDF,0xD6,0xD1,0xC4,0xC3,0xCA,0xCD, 
		0x90,0x97,0x9E,0x99,0x8C,0x8B,0x82,0x85,0xA8,0xAF,0xA6,0xA1,0xB4,0xB3,0xBA,0xBD, 
		0xC7,0xC0,0xC9,0xCE,0xDB,0xDC,0xD5,0xD2,0xFF,0xF8,0xF1,0xF6,0xE3,0xE4,0xED,0xEA, 
		0xB7,0xB0,0xB9,0xBE,0xAB,0xAC,0xA5,0xA2,0x8F,0x88,0x81,0x86,0x93,0x94,0x9D,0x9A, 
		0x27,0x20,0x29,0x2E,0x3B,0x3C,0x35,0x32,0x1F,0x18,0x11,0x16,0x03,0x04,0x0D,0x0A, 
		0x57,0x50,0x59,0x5E,0x4B,0x4C,0x45,0x42,0x6F,0x68,0x61,0x66,0x73,0x74,0x7D,0x7A, 
		0x89,0x8E,0x87,0x80,0x95,0x92,0x9B,0x9C,0xB1,0xB6,0xBF,0xB8,0xAD,0xAA,0xA3,0xA4, 
		0xF9,0xFE,0xF7,0xF0,0xE5,0xE2,0xEB,0xEC,0xC1,0xC6,0xCF,0xC8,0xDD,0xDA,0xD3,0xD4, 
		0x69,0x6E,0x67,0x60,0x75,0x72,0x7B,0x7C,0x51,0x56,0x5F,0x58,0x4D,0x4A,0x43,0x44, 
		0x19,0x1E,0x17,0x10,0x05,0x02,0x0B,0x0C,0x21,0x26,0x2F,0x28,0x3D,0x3A,0x33,0x34, 
		0x4E,0x49,0x40,0x47,0x52,0x55,0x5C,0x5B,0x76,0x71,0x78,0x7F,0x6A,0x6D,0x64,0x63, 
		0x3E,0x39,0x30,0x37,0x22,0x25,0x2C,0x2B,0x06,0x01,0x08,0x0F,0x1A,0x1D,0x14,0x13,
		0xAE,0xA9,0xA0,0xA7,0xB2,0xB5,0xBC,0xBB,0x96,0x91,0x98,0x9F,0x8A,0x8D,0x84,0x83, 
		0xDE,0xD9,0xD0,0xD7,0xC2,0xC5,0xCC,0xCB,0xE6,0xE1,0xE8,0xEF,0xFA,0xFD,0xF4,0xF3 
	}; 
	
	
	unsigned	char crc8_calculate(unsigned char * ucPtr, unsigned char ucLen) 
	{ 
		unsigned char ucCRC8 = 0;
	
		while(ucLen--)
		{ 
			ucCRC8 = crc8_tab[ucCRC8^(*ucPtr++)]; 
		} 
		return (~ucCRC8)&0xFF; 
	
	} 



	
	//��ʼ��IO ����2 
	//bound:������
	void uart2_init(u32 bound){
	
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	
	//1.����ʱ�Ӻͺ�GPIOʱ��ʹ�ܡ�
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��
	//2.�������Ÿ�����ӳ�� 
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA2����ΪUSART2
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA3����ΪUSART2  
	//3.GPIO�˿�ģʽ����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA2��GPIOA3��ʼ��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //�ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��GPIOA2����GPIOA3
	//4.���ڲ�����ʼ�������ò����ʣ��ֳ�����żУ��Ȳ���
	USART_InitStructure.USART_BaudRate = bound;//������һ������Ϊ9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2����
	//5.��ʼ��NVIC
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ��VIC�Ĵ���
	//6.�����ж�
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�  
	//7.ʹ�ܴ��� 
	USART_Cmd(USART2, ENABLE);					  //ʹ�ܴ��� 
	
		
	}
	
	
	
	int USART2_dataLen = -1; // json�ַ����ĳ���
	u8 USART2_jsonBuF[300]; // ���жϵ�ʱ�� �洢���յ�json �ַ���
	int USART2_jsonDataCount = 0; //��ǰ���յ� json �ַ�����
	u8 USART2_Car1_jsonParseBuF[300]; 
	int uart2ByteNum = 0; // ����2 ���շ���Э����ֽ���Ŀ
	u8 uart2CRC =0;
	
	void USART2StateTo0(void)
	{
		// �ָ���ʼ��
		USART2_dataLen = -1; // json�ַ����ĳ���
		memset(USART2_jsonBuF, 0, sizeof(USART2_jsonBuF));
		USART2_jsonDataCount = 0; //��ǰ���յ� json �ַ�����
		uart2ByteNum = 0;
		uart2CRC =0;
	
	}
	
	void USART2_IRQHandler(void)					//����2�жϷ������
	{
	
		u8 temp;
	
	
		if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
		{
			temp =USART2->DR;
	
			// ��һ���ֽ�
			if (uart2ByteNum == 0)
			{
				if (temp == '#')
				{
					uart2ByteNum++;
					//printf("\r\n get 1!!");
				}
				return ;
			}
			
			// �ڶ����ֽ�
			if (uart2ByteNum == 1)
			{
				if (temp == '!')
				{
					uart2ByteNum++;
					//printf("\r\n get 2!!");
					
				}
				else
				{
					uart2ByteNum = 0;
					//printf("\r\n get 2 FAILED!!");
					
				}
	  
				return ;
			}
			
			
			// ���� json Len ���ֽ�
			// ��3���ֽ�
			if (uart2ByteNum == 2)
			{
				USART2_dataLen = temp*256;
				uart2ByteNum++;
				return ;
			}
			
			// ���� json Len ���ֽ�
			// ��4���ֽ�
			if (uart2ByteNum == 3)
			{
				USART2_dataLen = USART2_dataLen + temp;
				uart2ByteNum++;
				//printf("\r\n get 6!!");
				return ;
			}
			
			// ��ʼ����
			if (uart2ByteNum == 4)
			{
				USART2_jsonDataCount++;
				
				if (USART2_jsonDataCount>250)  //  ���ܵĳ������
				{
					USART2StateTo0();
					return;
				}
							
				if (USART2_jsonDataCount <= USART2_dataLen)
				{
					USART2_jsonBuF[USART2_jsonDataCount-1] = temp;
					return;
				}
	
	
				// ĩβ��һ��У���ǩ
				if (USART2_jsonDataCount ==(USART2_dataLen + 1))
					{
						if (temp != '*')
							{
								USART2StateTo0();
							}
	
						return ;
					}
	
	
				// ĩβCRC
				if (USART2_jsonDataCount ==(USART2_dataLen + 2))
					{
						uart2CRC = temp;
						return ;
					}
	
	
				// ĩβ�ڶ���У���ǩ
				if (USART2_jsonDataCount ==(USART2_dataLen + 3))
					{
						if (temp != '&')
						{
							USART2StateTo0();
							
						}
						else
						{
							if ( uart2CRC == crc8_calculate(USART2_jsonBuF, USART2_dataLen) )
								{
									memset(USART2_Car1_jsonParseBuF, 0, sizeof(USART2_Car1_jsonParseBuF));
									strcpy(USART2_Car1_jsonParseBuF,USART2_jsonBuF);
									PaserCar1_State();
								}
							
							USART2StateTo0();	
							
						}
	
						return ;
			
					}
				return ;
			}
	
		}
	
	} 
		
	//////////////////////////////////////////////////////////////////
	/**************************ʵ�ֺ���**********************************************
	*��	��:		usart2����һ���ֽ�
	*********************************************************************************/
	void usart2_send(u8 data)
	{
		USART2->DR = data;
		while((USART2->SR&0x40)==0);	
	}
	
	
	/**************************ʵ�ֺ���**********************************************
	*��	��:		usart2����һ���ַ���
	*********************************************************************************/
	void usart2_sendString(char *data,u8 len)
	{
		int i=0;
		for(i=0;i<len;i++)
		{
			USART2->DR = data[i];
			while((USART2->SR&0x40)==0);	
		}
		
	}



	
	
	//��ʼ��IO ����4 
	//bound:������
	void uart4_init(u32 bound){
	
		NVIC_InitTypeDef NVIC_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
	
	
		//1.����ʱ�Ӻͺ�GPIOʱ��ʹ�ܡ�
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIOCʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);//ʹ��UART4ʱ��
		//2.�������Ÿ�����ӳ�� 
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); //GPIOC11����ΪUART4
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4); //GPIOC10����ΪUART4  
		//3.GPIO�˿�ģʽ����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10; //GPIOC11��GPIOC10��ʼ��
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //�ٶ�50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
		GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��GPIOC11����GPIOC10
		//4.���ڲ�����ʼ�������ò����ʣ��ֳ�����żУ��Ȳ���
		USART_InitStructure.USART_BaudRate = bound;//������һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
		USART_Init(UART4, &USART_InitStructure); //��ʼ������2����
		//5.��ʼ��NVIC
		NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ��VIC�Ĵ���
		//6.�����ж�
		USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//�����ж�  
		//7.ʹ�ܴ��� 
		USART_Cmd(UART4, ENABLE);					 //ʹ�ܴ��� 
	
		
	}
	
	
	
	int USART4_dataLen = -1; // json�ַ����ĳ���
	u8 USART4_jsonBuF[200]; // ���жϵ�ʱ�� �洢���յ�json �ַ���
	int USART4_jsonDataCount = 0; //��ǰ���յ� json �ַ�����
	u8 USART4_Getter_jsonParseBuF[200]; 
	int uart4ByteNum = 0; // ����2 ���շ���Э����ֽ���Ŀ
	u8 uart4CRC =0;
	
	void USART4StateTo0(void)
	{
		// �ָ���ʼ��
		USART4_dataLen = -1; // json�ַ����ĳ���
		memset(USART4_jsonBuF, 0, sizeof(USART4_jsonBuF));
		USART4_jsonDataCount = 0; //��ǰ���յ� json �ַ�����
		uart4ByteNum = 0;
		uart4CRC =0;
	
	}
	void UART4_IRQHandler(void) 				//����4�жϷ������
	{
	
		u8 temp;
	
		
		if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET) 
		{
			
			temp =UART4->DR;
	
			// ��һ���ֽ�
			if (uart4ByteNum == 0)
			{
				if (temp == '#')
				{
					uart4ByteNum++;
					//printf("\r\n get 1!!");
				}
				return ;
			}
			
			// �ڶ����ֽ�
			if (uart4ByteNum == 1)
			{
				if (temp == '!')
				{
					uart4ByteNum++;
					//printf("\r\n get 2!!");
					
				}
				else
				{
					uart4ByteNum = 0;
					//printf("\r\n get 2 FAILED!!");
					
				}
	  
				return ;
			}
			
			
			// ���� json Len ���ֽ�
			// ��3���ֽ�
			if (uart4ByteNum == 2)
			{
				USART4_dataLen = temp*256;
				uart4ByteNum++;
				return ;
			}
			
			// ���� json Len ���ֽ�
			// ��4���ֽ�
			if (uart4ByteNum == 3)
			{
				USART4_dataLen = USART4_dataLen + temp;
				uart4ByteNum++;
				//printf("\r\n get 6!!");
				return ;
			}
			
			// ��ʼ����
			if (uart4ByteNum == 4)
			{
				USART4_jsonDataCount++;
				
				if (USART4_jsonDataCount>250)  //  ���ܵĳ������
				{
					USART4StateTo0();
					return;
				}
							
				if (USART4_jsonDataCount <= USART4_dataLen)
				{
					USART4_jsonBuF[USART4_jsonDataCount-1] = temp;
					return;
				}
	
	
				// ĩβ��һ��У���ǩ
				if (USART4_jsonDataCount ==(USART4_dataLen + 1))
					{
						if (temp != '*')
							{
								USART4StateTo0();
							}
	
						return ;
					}
	
	
				// ĩβCRC
				if (USART4_jsonDataCount ==(USART4_dataLen + 2))
					{
						uart4CRC = temp;
						return ;
					}
	
	
				// ĩβ�ڶ���У���ǩ
				if (USART4_jsonDataCount ==(USART4_dataLen + 3))
					{
						if (temp != '&')
						{
							USART4StateTo0();
							
						}
						else
						{
							if ( uart4CRC == crc8_calculate(USART4_jsonBuF, USART4_dataLen) )
								{
									memset(USART4_Getter_jsonParseBuF, 0, sizeof(USART4_Getter_jsonParseBuF));
									strcpy(USART4_Getter_jsonParseBuF,USART4_jsonBuF);
									PaserGoodsGetter_State();
								}
							
							USART4StateTo0();	
							
						}
	
						return ;
			
					}
				return ;
			}
			
		
			
	
		}
	} 
		
	//////////////////////////////////////////////////////////////////
	/**************************ʵ�ֺ���**********************************************
	*��	��:		uart4����һ���ֽ�
	*********************************************************************************/
	void uart4_send(u8 data)
	{
		UART4->DR = data;
		while((UART4->SR&0x40)==0); 
	}
	
	
	/**************************ʵ�ֺ���**********************************************
	*��	��:		uart4����һ���ַ���
	*********************************************************************************/
	void uart4_sendString(char *data,u8 len)
	{
		int i=0;
		for(i=0;i<len;i++)
		{
			UART4->DR = data[i];
			while((UART4->SR&0x40)==0); 
		}
		
	}
	
	
	
	
	//��ʼ��IO ����5 
	//bound:������
	void uart5_init(u32 bound){
	
		NVIC_InitTypeDef NVIC_InitStructure;
		GPIO_InitTypeDef GPIO_InitStructure;
		USART_InitTypeDef USART_InitStructure;
	
	
		//1.����ʱ�Ӻͺ�GPIOʱ��ʹ�ܡ�
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE); //ʹ��GPIOCʱ��
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE); //ʹ��GPIODʱ��
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);//ʹ��UART5ʱ��
		//2.�������Ÿ�����ӳ�� 
		GPIO_PinAFConfig(GPIOC,GPIO_PinSource12,GPIO_AF_UART5); //GPIOC12����ΪUART5
		GPIO_PinAFConfig(GPIOD,GPIO_PinSource2,GPIO_AF_UART5); //GPIOD2����ΪUART5  
		//3.GPIO�˿�ģʽ����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //GPIOC12��ʼ��
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //�ٶ�50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
		GPIO_Init(GPIOC,&GPIO_InitStructure); //��ʼ��GPIOC12
	
		//3.GPIO�˿�ģʽ����
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //GPIOD2��ʼ��
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; //�ٶ�50MHz
		GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
		GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
		GPIO_Init(GPIOD,&GPIO_InitStructure); //��ʼ��GPIOD2
	
		
		//4.���ڲ�����ʼ�������ò����ʣ��ֳ�����żУ��Ȳ���
		USART_InitStructure.USART_BaudRate = bound;//������һ������Ϊ9600;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
		USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
		USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
		USART_Init(UART5, &USART_InitStructure); //��ʼ������2����
		//5.��ʼ��NVIC
		NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ��ʹ��
		NVIC_Init(&NVIC_InitStructure); //����ָ���Ĳ�����ʼ��VIC�Ĵ���
		//6.�����ж�
		USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);//�����ж�  
		//7.ʹ�ܴ��� 
		USART_Cmd(UART5, ENABLE);					 //ʹ�ܴ��� 
	
		
	}
	
	
	int USART5_dataLen = -1; // json�ַ����ĳ���
	u8 USART5_jsonBuF[300]; // ���жϵ�ʱ�� �洢���յ�json �ַ���
	int USART5_jsonDataCount = 0; //��ǰ���յ� json �ַ�����
	u8 USART5_Car2_jsonParseBuF[300]; 
	int uart5ByteNum = 0; // ����2 ���շ���Э����ֽ���Ŀ
	u8 uart5CRC =0;
	
	void USART5StateTo0(void)
	{
		// �ָ���ʼ��
		USART5_dataLen = -1; // json�ַ����ĳ���
		memset(USART5_jsonBuF, 0, sizeof(USART5_jsonBuF));
		USART5_jsonDataCount = 0; //��ǰ���յ� json �ַ�����
		uart5ByteNum = 0;
		uart5CRC =0;
	
	}
	
	void UART5_IRQHandler(void) 				//����5�жϷ������
	{
	
		u8 temp;
	
		
		if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET) 
		{
			
			temp =UART5->DR;
	
			// ��һ���ֽ�
			if (uart5ByteNum == 0)
			{
				if (temp == '#')
				{
					uart5ByteNum++;
					//printf("\r\n get 1!!");
				}
				return ;
			}
			
			// �ڶ����ֽ�
			if (uart5ByteNum == 1)
			{
				if (temp == '!')
				{
					uart5ByteNum++;
					//printf("\r\n get 2!!");
					
				}
				else
				{
					uart5ByteNum = 0;
					//printf("\r\n get 2 FAILED!!");
					
				}
	  
				return ;
			}
			
			
			// ���� json Len ���ֽ�
			// ��3���ֽ�
			if (uart5ByteNum == 2)
			{
				USART5_dataLen = temp*256;
				uart5ByteNum++;
				return ;
			}
			
			// ���� json Len ���ֽ�
			// ��4���ֽ�
			if (uart5ByteNum == 3)
			{
				USART5_dataLen = USART5_dataLen + temp;
				uart5ByteNum++;
				//printf("\r\n get 6!!");
				return ;
			}
			
			// ��ʼ����
			if (uart5ByteNum == 4)
			{
				USART5_jsonDataCount++;
				
				if (USART5_jsonDataCount>250)  //  ���ܵĳ������
				{
					USART5StateTo0();
					return;
				}
							
				if (USART5_jsonDataCount <= USART5_dataLen)
				{
					USART5_jsonBuF[USART5_jsonDataCount-1] = temp;
					return;
				}
	
	
				// ĩβ��һ��У���ǩ
				if (USART5_jsonDataCount ==(USART5_dataLen + 1))
					{
						if (temp != '*')
							{
								USART5StateTo0();
							}
	
						return ;
					}
	
	
				// ĩβCRC
				if (USART5_jsonDataCount ==(USART5_dataLen + 2))
					{
						uart5CRC = temp;
						return ;
					}
	
	
				// ĩβ�ڶ���У���ǩ
				if (USART5_jsonDataCount ==(USART5_dataLen + 3))
					{
						if (temp != '&')
						{
							USART5StateTo0();
							
						}
						else
						{
							if ( uart5CRC == crc8_calculate(USART5_jsonBuF, USART5_dataLen) )
								{
									memset(USART5_Car2_jsonParseBuF, 0, sizeof(USART5_Car2_jsonParseBuF));
									strcpy(USART5_Car2_jsonParseBuF,USART5_jsonBuF);
									PaserCar2_State();
								}
							
							USART5StateTo0();	
							
						}
	
						return ;
			
					}
				return ;
			}
			
		
			
	
		}
	} 
		
	//////////////////////////////////////////////////////////////////
	/**************************ʵ�ֺ���**********************************************
	*��	��:		uart4����һ���ֽ�
	*********************************************************************************/
	void uart5_send(u8 data)
	{
		UART5->DR = data;
		while((UART5->SR&0x40)==0); 
	}
	
	
	/**************************ʵ�ֺ���**********************************************
	*��	��:		uart4����һ���ַ���
	*********************************************************************************/
	void uart5_sendString(char *data,u8 len)
	{
		int i=0;
		for(i=0;i<len;i++)
		{
			UART5->DR = data[i];
			while((UART5->SR&0x40)==0); 
		}
		
	}
	

