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

struct Slot_TDMA_Cand
{
	unsigned short slot_index:15;
	unsigned short chanel:1;
};

int Ra_TDMA(struct AIS_TDMA *LME);

void Ra_TDMA_main();
unsigned char Get_Slot_RATDMA(unsigned int start_slot,unsigned int slot_num,struct Slot_TDMA_Cand *slot_cand);

void wait_for_slot(unsigned int slot_index);
void send_message(unsigned int slot_index,unsigned short channel);


#endif

