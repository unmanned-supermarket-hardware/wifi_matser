#ifndef __AIWACSUPERMAKET_H
#define __AIWACSUPERMAKET_H

#include "malloc.h"
#include "cJSON.h"
#include "usart3.h"
#include  "stdio.h"
#include "stdlib.h"

#define MASTER_AREA_1 "Area1"
#define MASTER_AREA_2 "Area2"

#define MASTER_ID MASTER_AREA_1   // 烧录不同的主控的时候需要改



#define TURING_DISTANCE 0.52  // 前方需要转弯的  时候 m
#define TURING_DISTANCE_GAP 0.015  // 转弯区间 m

#define FRONT_DISTANCEGAP  0.02  //  两车  前后 距离差 m
#define DEFUALT_SPEED 100  //  小车的默认速度
#define MIN_SPEED 30  //小车 运动的最小速度   mm/s
#define ACC_SPEED 150  //  小车的加速速度
#define CAR_STOP 0  // 小车速度停止
#define FD_MAX_SPEED 600  // 小车运动的 最大速度  mm/s

// 小车的运动状态
#define STATE_STOP 0
#define STATE_STRAIGHT 1
#define STATE_TURN_RIGHT 2
#define STATE_TURN_LEFT 3



// 小车前进方向用
#define FRONT_DIRECTION  1
#define BACK_DIRECTION  2


#define MIDDLE_SPACE 2

#define AIWAC_IP "192.168.3.158"
//用户配置参数
extern const u8* portnum;			//连接端口
 
extern const u8* wifista_ssid;		//WIFI STA SSID
extern const u8* wifista_encryption;//WIFI STA 加密方式
extern const u8* wifista_password; 	//WIFI STA 密码
extern int  printfNUM ; //打印计数
extern int LocationNow;


// 全局存储  系统的  状态
struct systemState {
	int car1State;
	char car1Error[100];
	int car2State;
	char car2Error[100];
	int goodsGetterState;
	char goodsGetterEorror[100];
	int masterState;  
	char[200] allErrorDesc;  // 嵌入式的状态
}  ;
extern struct systemState SystemState;


// 存储安卓端下发的  货物位置信息
struct goodsLocation {
	char side[3];
	char distance[8];
	char height[8];
	char depth[8];

}  ;
extern struct goodsLocation GoodsLocation;


extern int Car1_CorrectState ;
extern double Car1_FDistance ;
extern double Car1_BDistance ;
extern int Car1_moveState;

extern int Car2_CorrectState ;
extern double Car2_FDistance ;
double Car2_BDistance ;
extern int Car2_moveState ;


extern int GotGoodsResult;
extern int LoseGoodsResult;
extern int LosePanResult;



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
void feedbackGotOrder(void);
void feedbackStartGetGoods(void);
void notifyGoodsGetterLocation(void);
void waitingGetterGotGoods(void);
void feedbackGotGoodsResult(void);
void controlCarToGate(void);
void notifyGoodsGetterLoseGoods(void );
void waitingGetterLoseGoods(void);
void feedbackLoseGoodsResult(void);
void controlCarToDropPan(void);
void notifyGoodsGetterDropPan(void );
void waitingGetterLosePan(void);
void controlCarToInitSpace(void);
void feedbackGoInit(void);
void goGoalPosition(int direction,double needDistance);
void goStartTogether(int direction);
void goToLocation(int direction,double needDistance);
double  designFSpeed2(double FD, double FD_care,double iniTDistance);
void sendTuringOrder(int Left_or_Right);
void AiwacMasterSendOrderCar1(double X_V, int moveState);
void AiwacMasterSendOrderCar2(double X_V, int moveState);



  


#endif
