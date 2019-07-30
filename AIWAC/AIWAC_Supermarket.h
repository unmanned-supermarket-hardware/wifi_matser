#ifndef __AIWACSUPERMAKET_H
#define __AIWACSUPERMAKET_H

#include "malloc.h"
#include "cJSON.h"
#include "usart3.h"
#include  "stdio.h"
#include "stdlib.h"


#define MIDDLE_SPACE 2

#define AIWAC_IP "192.168.3.158"
//用户配置参数
extern const u8* portnum;			//连接端口
 
extern const u8* wifista_ssid;		//WIFI STA SSID
extern const u8* wifista_encryption;//WIFI STA 加密方式
extern const u8* wifista_password; 	//WIFI STA 密码
extern int  printfNUM ; //打印计数

// 全局存储  系统的  状态
struct systemState {
	int car1State;
	char car1Error[100];
	int car2State;
	char car2Error[100];
	int goodsGetterState;
	char goodsGetter[100];
	int masterState;  
	char[200] allErrorDesc;  // 嵌入式的状态
}  ;
extern struct systemState SystemState;


extern int Car1_CorrectState ;
extern double Car1_FDistance ;
extern double Car1_BDistance ;
extern int Car1_moveState;

extern int Car2_CorrectState ;
extern double Car2_FDistance ;
double Car2_BDistance ;
extern int Car2_moveState ;




void initSysValue(void);
void askState2other(void );
void wifi_Init(void);
void parseOrderFromS(int goalType);
void WIFISend(char* MS);
void  AIWAC_MasterGetGoods(void);
void waitingSAskState(void);
void waitingSSendLocation(void);
void DropGoods(void);
void DropPan(void);
void PaserCar1_State(void);
void PaserCar2_State(void);
void PaserGoodsGetter_State(void);
double myabs_double(double a);

  


#endif
