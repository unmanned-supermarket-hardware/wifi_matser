#ifndef __AIWACSUPERMAKET_H
#define __AIWACSUPERMAKET_H

#include "malloc.h"
#include "cJSON.h"
#include "usart3.h"
#include  "stdio.h"

extern int WIFIInitOK;
extern int numF1;
extern	int numF2;

#define AIWAC_IP "192.168.3.158"
//用户配置参数
extern const u8* portnum;			//连接端口
 
extern const u8* wifista_ssid;		//WIFI STA SSID
extern const u8* wifista_encryption;//WIFI STA 加密方式
extern const u8* wifista_password; 	//WIFI STA 密码

void wifi_Init(void);
int parseOrderFromS(void);
void WIFISend(char* MS);
void  AIWAC_MasterGetGoods(void);
void waitingSAskState(void);
void waitingSSendLocation(void);
void DropGoods(void);
void DropPan(void);




#endif
