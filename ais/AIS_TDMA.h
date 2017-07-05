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

