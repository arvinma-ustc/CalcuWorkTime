#ifndef _AIS_TDMA_H
#define _AIS_TDMA_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

struct SLOT_S
{
	unsigned char chb_a:4;   //�ŵ�a ״̬   ����ʱ϶5������
	unsigned char chb_b:4;   //�ŵ�b ״̬
   unsigned char chb_S:1;   //�����ŵ�ѡ��   0=a�ŵ���1=b�ŵ�
   unsigned char outTime:4; //��ʱ���� 3-7����
   unsigned char pro:3;     //ѡ���͵�Э��  ais=0��asm=1��vde25k=2��vde50k=3��vde100k=4
   //unsigned char outTime;
   unsigned  short back:3;  //����=0������0-7��
   unsigned  char  rtp;     //��ǰ����0-100
   unsigned  short TaskNo:13;  //������
};

struct AIS_TDMA
{
	unsigned short RTES;	//����ʱ϶0-2249
	unsigned char RTCSC;  	//��ѡʱ϶1-150
	unsigned char RTPRI:2;	//���ȼ�1-0
	unsigned char RTPS:6;	//��ʼ����0-25
	unsigned char RTP1;		//��������0-100
	unsigned char RTP2;		//��ǰ����RTPS-100
	unsigned char RTA;		//���Դ���0-149
	unsigned char RTPI;		//��������1-25
	unsigned short ITING;	//ʱ϶����0-8191��ITMDA
	unsigned char ITSL:3;	//ʱ϶��1-5
	unsigned char ITKP:1;	//���ֱ�־
};

struct AIS_ITDMA
{
	unsigned short ITING;	//ʱ϶����0-8191��ITMDA
	unsigned char ITKP:1;	//���ֱ�־
	unsigned char CHANNEL_NEXT:1;	//���ֱ�־
	unsigned char ITSL:3;	//ʱ϶��1-5
};

struct AIS_SOTDMA
{
	unsigned short 	NSS;	//��ƿ�ʼʱ϶0-2249
	unsigned short 	NS;	//���ʱ϶0-2249
	unsigned short 	NTS;	//��ƴ���ʱ϶0-2249
	unsigned char  Rr:5;	//����Ƶ��2-30
	unsigned char  TIME_OUT:3;	//ʱ϶��ʱ
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

