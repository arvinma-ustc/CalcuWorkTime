#ifndef _AIS_TDMA_H
#define _AIS_TDMA_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

struct SLOT_S
{
	unsigned char chb_a:4;   //信道a 状态   用于时隙5条规则
	unsigned char chb_b:4;   //信道b 状态
   unsigned char chb_S:1;   //发送信道选择   0=a信道，1=b信道
   unsigned char outTime:4; //超时计数 3-7分钟
   unsigned char pro:3;     //选择发送的协议  ais=0，asm=1，vde25k=2，vde50k=3，vde100k=4
   //unsigned char outTime;
   unsigned  short back:3;  //单块=0，连块0-7块
   unsigned  char  rtp;     //当前概率0-100
   unsigned  short TaskNo:13;  //任务编号
};

struct AIS_TDMA
{
	unsigned short RTES;	//结束时隙0-2249
	unsigned char RTCSC;  	//候选时隙1-150
	unsigned char RTPRI:2;	//优先级1-0
	unsigned char RTPS:6;	//开始概率0-25
	unsigned char RTP1;		//导出概率0-100
	unsigned char RTP2;		//当前概率RTPS-100
	unsigned char RTA;		//尝试次数0-149
	unsigned char RTPI;		//概率增量1-25
	unsigned short ITING;	//时隙增量0-8191，ITMDA
	unsigned char ITSL:3;	//时隙数1-5
	unsigned char ITKP:1;	//保持标志
};

struct AIS_ITDMA
{
	unsigned short ITING;	//时隙增量0-8191，ITMDA
	unsigned char ITKP:1;	//保持标志
	unsigned char CHANNEL_NEXT:1;	//保持标志
	unsigned char ITSL:3;	//时隙数1-5
};

struct AIS_SOTDMA
{
	unsigned short 	NSS;	//标称开始时隙0-2249
	unsigned short 	NS;	//标称时隙0-2249
	unsigned short 	NTS;	//标称传输时隙0-2249
	unsigned char  Rr:5;	//报告频次2-30
	unsigned char  TIME_OUT:3;	//时隙超时
};


struct CHAR256 
{
	char data[32];
};

struct A_BSO_B_MESSAGE 
{
	char slop;
	char train[3];
	char startflag;
	char data[21];
	char FCS[2];
	char endflag;
	char buffer[3];
};

/*
struct A_BSO_B_MESSAGE 
{
	struct CHAR256 slop:8;
	struct CHAR256 train:24;
	struct CHAR256 startflag:8;
	struct CHAR256 MSGID:6;
	struct CHAR256 UserID:30;
	struct CHAR256 data:113;
	struct CHAR256 communistatus:19;
	struct CHAR256 FCS:16;
	struct CHAR256 endflag:8;
	struct CHAR256 buffer:24;
};*/



struct Slot_TDMA_Cand
{
	unsigned short slot_index:15;
	unsigned short chanel:1;
};

int Ra_TDMA(struct AIS_TDMA *LME);

void Ra_TDMA_main();
void wait_for_slot(unsigned int slot_index);
void send_message(unsigned int slot_index,unsigned short channel);

unsigned char Get_Slot_ITDMA(unsigned int slot_start,unsigned int slot_NI,unsigned int slot_num,struct Slot_TDMA_Cand *slot_cand);
unsigned char Get_Slot_RATDMA(unsigned int slot_num,struct Slot_TDMA_Cand *slot_cand);
void I_TDMA(unsigned int slot_start,unsigned int slot_NI,struct AIS_ITDMA *LME,unsigned int slot_send);


#endif

