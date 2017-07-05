#include "AIS_TDMA.h"
#include "selectslot.h"
#include "main.h"



/*全局变量定义在main.c中*/
extern struct SLOT_S g_slot[MAX_SLOT_COUNT];
extern unsigned short int g_slot_index;


void Ra_TDMA_main()
{
	struct AIS_TDMA LME;
	struct Slot_TDMA_Cand *slot_cand;
	char slot_index=0;

	srand((unsigned)time(NULL)); //用时间做种，每次产生随机数不一样
	slot_cand = (struct Slot_TDMA_Cand*)malloc(4*sizeof(struct Slot_TDMA_Cand));
	LME.RTCSC=Get_Slot_RATDMA(g_slot_index,4,slot_cand);
	LME.RTPS=100/LME.RTCSC;
	LME.RTP2=LME.RTPS;
	LME.RTA=0;
	slot_index=Ra_TDMA(&LME);
	wait_for_slot(slot_cand[slot_index].slot_index);
	send_message(slot_cand[slot_index].slot_index,slot_cand[slot_index].chanel);
}

/*	从start_slot开始的150个时隙中根据时隙候选规则选取slot_num个备选时隙
,并将选取的结果存储在slot_cand指针指向的结构体中*/
unsigned char Get_Slot_RATDMA(unsigned int start_slot,unsigned int slot_num,struct Slot_TDMA_Cand *slot_cand)
{
	struct select_interval_struct interval;
	struct slot_candidate_struct candidate;
	unsigned char index_slot=0,tmp=0;

	interval.start=start_slot;
	interval.end=interval.start+150;
	interval.nominal_slot=interval.start+75;
	interval.report_index=1;
	interval.report_rate=2;//does not use just for parament check

	candidate.cadidate_sum=0;
	candidate.report_index = 1;
	candidate.report_rate = 2;
	candidate.slot_length=1;
/*select candidate slot from channel A*/
	Get_Candidateslot_from_Interval(&interval,&candidate,CHANNEL_A);
	for(index_slot=0;index_slot < candidate.cadidate_sum && index_slot < slot_num;index_slot++)
	{
		slot_cand[index_slot].slot_index = candidate.slot_index[index_slot];
		slot_cand[index_slot].chanel = 0;
	}
/* if can not find enough slot then select from channel B*/
	if(candidate.cadidate_sum < slot_num)
	{
		candidate.cadidate_sum=0;
		Get_Candidateslot_from_Interval(&interval,&candidate,CHANNEL_B);
		for(tmp=0;tmp< candidate.cadidate_sum && index_slot < slot_num;tmp++)
		{
			slot_cand[index_slot].slot_index = candidate.slot_index[tmp];
			slot_cand[index_slot].chanel = 1;
			index_slot++;
		}
	}

	return index_slot;
}


/*
return: -1 表示候选时隙错误
概率p持续算法
*/
int Ra_TDMA(struct AIS_TDMA *LME)
{
	char index_csc=0;

	if(LME->RTCSC < 1)
		return -1;
	while(LME->RTCSC > 0)
	{
		LME->RTP1= rand() % 101;
	 	if(LME->RTP1 > LME->RTP2)
   		{
	   		LME->RTPI=(100-LME->RTP2)/LME->RTCSC;
	   		LME->RTP2 += LME->RTPI;
			LME->RTCSC-=1;//order
			LME->RTA+=1;
			index_csc++;
   		}
   		else
   		{		
			LME->RTCSC-=1;//order
			LME->RTA+=1;
			return index_csc;
   		}
	}
	return -1;

}


void wait_for_slot(unsigned int slot_index)
{
	return;
}

void send_message(unsigned int slot_index,unsigned short channel)
{
	printf("\nsend message in the:%d  on the channel:%c \n", slot_index, channel?'B':'A');
	return;
}

