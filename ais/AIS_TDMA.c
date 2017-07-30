#include "AIS_TDMA.h"
#include "selectslot.h"
#include "main.h"



/*全局变量定义在main.c中*/
extern struct SLOT_S g_slot[MAX_SLOT_COUNT];
extern unsigned short int g_slot_index;
extern void *g_message;
extern unsigned char g_cur_Rr;


void Ra_TDMA_main()
{
	struct AIS_TDMA LME;
	struct Slot_TDMA_Cand *slot_cand;
	char slot_index=0;

	srand((unsigned)time(NULL)); //用时间做种，每次产生随机数不一样
	slot_cand = (struct Slot_TDMA_Cand*)malloc(4*sizeof(struct Slot_TDMA_Cand));
	LME.RTCSC=Get_Slot_RATDMA(4,slot_cand);
	LME.RTPS=100/LME.RTCSC;
	LME.RTP2=LME.RTPS;
	LME.RTA=0;
	slot_index=Ra_TDMA(&LME);
	if(slot_index >= 0)
	{
		wait_for_slot(slot_cand[slot_index].slot_index);
		send_message(slot_cand[slot_index].slot_index,slot_cand[slot_index].chanel,g_message);
	}
}

/*	从start_slot开始的150个时隙中根据时隙候选规则选取slot_num个备选时隙
,并将选取的结果存储在slot_cand指针指向的结构体中*/
unsigned char Get_Slot_RATDMA(unsigned int slot_num,struct Slot_TDMA_Cand *slot_cand)
{
	struct select_interval_struct interval;
	struct slot_candidate_struct candidate;
	unsigned char index_slot=0,tmp=0;

	interval.start=g_slot_index;
	interval.end=interval.start+150;
	interval.nominal_slot=interval.start+75;
	interval.report_index=1;
	interval.report_rate=2;//does not use just for parament check

	candidate.cadidate_sum=0;
	candidate.report_index = 1;
	candidate.report_rate = 2;
	candidate.slot_length=1;
	while(index_slot < 1)
	{
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
		/*if can`t find any candidate slot in this SI, then move to next SI(150) */
		//interval.start  need boundary check 
		if(index_slot < 1)
			{
			interval.start+=150;
			interval.end+=150;
			interval.nominal_slot+=150;
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

void send_message(unsigned int slot_index,unsigned short channel,void *data)
{
	printf("\nsend message in the:%d  on the channel:%c \n", slot_index, channel?'B':'A');
	return;
}

//for ITDMA


/*I_TDMA select next slot and */

void Set_ITDMA_Flag(struct AIS_ITDMA *LME,void *message)
{
//set ITDMA Flag
//add LME->ITING to message
//modify g_slots
}

void I_TDMA(unsigned int slot_start,unsigned int slot_NI,struct AIS_ITDMA *LME,unsigned int slot_send)
{
	unsigned int slot_next;
	
	struct Slot_TDMA_Cand * slot_cand;
	slot_cand=(struct Slot_TDMA_Cand*)malloc(4*sizeof(struct Slot_TDMA_Cand));
	
	slot_next = Get_Slot_ITDMA(slot_start,slot_NI,LME->ITSL,slot_cand);//slot_next use as a temp value here
	if(slot_next > 0)
	{
		slot_next = rand() % slot_next;
		LME->CHANNEL_NEXT = slot_cand[slot_next].chanel;
		slot_next = slot_cand[slot_next].slot_index;//choice next send slot
	}
	LME->ITING = MAX_SLOT_COUNT + slot_next - slot_send; //plus MAX_SLOT_COUNT mean next frame

	Set_ITDMA_Flag(LME,g_message);	
}

unsigned char Get_Slot_ITDMA(unsigned int slot_start,unsigned int slot_NI,unsigned int slot_num,struct Slot_TDMA_Cand *slot_cand)
{
	struct select_interval_struct interval;
	struct slot_candidate_struct candidate;
	unsigned char index_slot=0,tmp=0;

	interval.start=slot_start;
	interval.end=interval.start+slot_NI/5;
	interval.nominal_slot=interval.start+slot_NI/10;
	interval.report_index=1;
	interval.report_rate=2;//does not use just for parament check

	candidate.cadidate_sum=0;
	candidate.report_index = 1;
	candidate.report_rate = 2;
	candidate.slot_length=slot_num;
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




//for SOTDMA 

int SOTDMA(unsigned short NSS,unsigned char Rr)
{

//check the parameter
	if(NSS > 2249 || NSS < 0)
		return -1;
	if(Rr > 30 || Rr < 2)
		return -2;

//compare argument with global variable check if change report rate 
	if(Rr == g_cur_Rr)
	//continue stage
	SOTDMA_Contin(NSS,Rr);
	else
	//update Rr stage
	SOTDMA_Update(NSS,Rr);


}

void SOTDMA_Contin(unsigned short NSS,unsigned char Rr)
{
//local variable
unsigned short	NS; 		//nominal slot
unsigned short	NI; 		//nominal increment
unsigned short	SI_begin;	//select interval begin
unsigned short	SI_end; 	//select interval end
unsigned short	NTS;		//nomial transmission slot
unsigned short	NTS_New;	//next nomial transmission slot
unsigned short	offset; 	//offset
unsigned char	TMO;		//time out 3-7
unsigned char	Rr_index;	//report rate index



	NI = 2250/Rr;
	while(Rr == g_cur_Rr)
	{
		//calculate current Rr_index
		if(g_slot_index < NSS)
			Rr_index=(g_slot_index + MAX_SLOT_COUNT - NSS)/NI;
		else
			Rr_index=(g_slot_index - NSS)/NI;

		//calculate next SI
		Rr_index =	(Rr_index + 1)%Rr;
		NS = NSS + (Rr_index * NI);
		SI_begin = (NS - (NI/10))%MAX_SLOT_COUNT;
		SI_end = (NS + (NI/10))%MAX_SLOT_COUNT;

		//check stage

		NTS = Get_NTS(SI_begin,SI_end);
		//waiting for NTS
		wait_for_slot(NTS);
		//decrease time out
		TMO = --g_slot[NTS].outTime;
		//check time out equal to zero 
		if(TMO == 0)
			{
			//get new NTS
			NTS_New=Get_New_NTS_SOTDMA(NTS,NI);
			//error deal 
			if(NTS_New == MAX_SLOT_COUNT)
				break;
			//calculate new offset and new time out
			offset = (NTS_New - NTS)%MAX_SLOT_COUNT;
			TMO = TMO_MIN + rand()%(TMO_MAX + 1 - TMO_MIN);
			g_slot[NTS_New].outTime = TMO;
			}	
		else
			{
			//set offset to zero
			offset = 0;
			}
		//add new time out and offset to message
		add_TmoOF_to_Message(TMO,offset,g_message);
		//send message
		send_message(NTS,g_slot[NTS].chb_S,g_message);
	}	

}

void SOTDMA_Update(unsigned short NSS,unsigned char Rr)
{
//local variable
struct AIS_ITDMA LME;

unsigned short	NS; 		//nominal slot
unsigned short	NI; 		//nominal increment
unsigned short	SI_begin;	//select interval begin
unsigned short	SI_end; 	//select interval end
unsigned short	NTS;		//nomial transmission slot
unsigned short	NTS_Next;	//next nomial transmission slot
unsigned short	offset; 	//offset
unsigned char	TMO;		//time out 3-7
unsigned char	Rr_index;	//report rate index

if(g_cur_Rr==0) //g_cur_Rr initialize 0 when definition and will change in the first frame
	return;

	NI = 2250/g_cur_Rr;
//calculate current Rr_index
	if(g_slot_index < NSS)
		Rr_index=(g_slot_index + MAX_SLOT_COUNT - NSS)/NI;
	else
		Rr_index=(g_slot_index - NSS)/NI;

	//calculate next SI
	Rr_index =	(Rr_index + 1)%g_cur_Rr;
	NS = NSS + (Rr_index * NI);
	SI_begin = (NS - (NI/10))%MAX_SLOT_COUNT;
	SI_end = (NS + (NI/10))%MAX_SLOT_COUNT;

	NTS = Get_NTS(SI_begin,SI_end);

	//waiting for NTS
	wait_for_slot(NTS);

	//set NSS to NS
	NSS = NS;
	NI = 2250/Rr;
	Rr_index = 0;
while(1)
{
	//check next si first calculate next SI
	Rr_index++;
	NS = (NSS+ NI*Rr_index)%MAX_SLOT_COUNT;
	SI_begin = (NS - (NI/10))%MAX_SLOT_COUNT;
	SI_end = (NS + (NI/10))%MAX_SLOT_COUNT;
	
	NTS_Next= Get_NTS(SI_begin,SI_end);
	if(NTS_Next == MAX_SLOT_COUNT)
		{
			LME.ITSL=g_slot[NTS].back;
			LME.ITKP=1;
			I_TDMA(SI_begin,NI,&LME,NTS);
		}
	else
		{
			//check NTS_NEXT is relate to NSS
			//IF it is then break 
			if(Check_NSS_NTS(NSS,NTS_Next,NI))
				break;
		}
	send_message(NTS,g_slot[NTS].chb_S,g_message);

	Wait_Next_SI(NS,NI,NTS_Next);
}	

	//


}

//waiting for next Select interval, check every slot in current SI if it status is STATUS_INTER
void Wait_Next_SI(unsigned short NS, unsigned short NI,unsigned short send_slot)
{
	unsigned short increat=0;
	for(increat=0;increat<(4*NI)/10;increat++)
		{
			if((g_slot[(NS+increat)%MAX_SLOT_COUNT].chb_a=STATUS_INTER)||
				(g_slot[(NS+increat)%MAX_SLOT_COUNT].chb_b=STATUS_INTER))
			{	add_TmoOF_to_Message(0,0,g_message);
				g_slot[(NS+increat)%MAX_SLOT_COUNT].outTime=0;
				send_message((NS+increat)%MAX_SLOT_COUNT,
					g_slot[(NS+increat)%MAX_SLOT_COUNT].chb_S,g_message);
				}
		}
	for(;increat<(6*NI)/10;increat++)
		{
			
		}
	for(;increat<NI;increat++)
		{
			if((g_slot[(NS+increat)%MAX_SLOT_COUNT].chb_a=STATUS_INTER)||
				(g_slot[(NS+increat)%MAX_SLOT_COUNT].chb_b=STATUS_INTER))
			{	add_TmoOF_to_Message(0,0,g_message);
				g_slot[(NS+increat)%MAX_SLOT_COUNT].outTime=0;
				send_message((NS+increat)%MAX_SLOT_COUNT,
					g_slot[(NS+increat)%MAX_SLOT_COUNT].chb_S,g_message);
				}
		}
	wait_for_slot(send_slot);
}

char Check_NSS_NTS(unsigned short NSS,unsigned short NTS,unsigned short NI)
{
	if(NSS < NI/10)
		{
		if((NTS < NSS + NI/10)||(NTS>(NSS + MAX_SLOT_COUNT - NI/10)))
			return 1;
		else
			return 0;
		}
	else if(NSS + NI/10 > MAX_SLOT_COUNT)
		{
		if((NTS > NSS - NI/10)||(NTS<(NSS + NI/10 - MAX_SLOT_COUNT)))
			return 1;
		else
			return 0;
		}			
	else
		{
		if((NTS > NSS - NI/10)&& NTS<(NSS + NI/10))
			return 1;
		else
			return 0;
		}
		
}

//find NTS from SI , if return MAX_SLOT_COUNT mean can`t find one, NTS mean slot status is STATUS_INTER 
unsigned short Get_NTS(unsigned short SI_begin,unsigned short SI_end)
{
	unsigned short slotindex;
	//check arguments
	if(SI_begin > (MAX_SLOT_COUNT-1) || SI_begin < 0)
		return MAX_SLOT_COUNT;
	if(SI_end > (MAX_SLOT_COUNT-1) || SI_end < 0)
		return MAX_SLOT_COUNT;
	
	if(SI_end < SI_begin)
		SI_end = SI_end + MAX_SLOT_COUNT;

	for(slotindex=SI_begin; slotindex<SI_end; slotindex++)
	{
		if((g_slot[slotindex%MAX_SLOT_COUNT].chb_a==STATUS_INTER)||
			(g_slot[slotindex%MAX_SLOT_COUNT].chb_b==STATUS_INTER))
			return (slotindex%MAX_SLOT_COUNT);
	}

	return MAX_SLOT_COUNT;
}

void add_TmoOF_to_Message(unsigned short TMO,unsigned short Offset,void *message)
{

}

unsigned char Get_New_NTS_SOTDMA(unsigned int slot_start,unsigned int slot_NI)
{
	struct select_interval_struct interval;
	struct slot_candidate_struct candidate;
	unsigned char index_slot=0;
	int channel;


	interval.start=slot_start;
	interval.end=interval.start+slot_NI/5;	//division 5 mean 20% 
	interval.nominal_slot=interval.start+slot_NI/10;	//division 10 mean 10%
	interval.report_index=1;
	interval.report_rate=2;//does not use just for parament check

	candidate.cadidate_sum=0;
	candidate.report_index = 1;	//small than report rate is ok
	candidate.report_rate = 2;	//just use for argument check
	candidate.slot_length=g_slot[slot_start].back;
	
	if(g_slot[slot_start].chb_S)
		channel = CHANNEL_B;
	else
		channel = CHANNEL_A;	

	Get_Candidateslot_from_Interval(&interval,&candidate,channel);
	index_slot=Selcet_Slot_from_Candidate(&candidate);
	if(index_slot==-1)
		index_slot==MAX_SLOT_COUNT;

	return index_slot;
}


//for FATDMA

void FATDMA(unsigned short start_slot,unsigned char increment,char size)
{
//argument check 
	if(start_slot > 2249 || start_slot < 0)
		return ;
	if(increment > 1225 || increment < 75)
		return ;
	if(size > 5 || size < 1)
		return ;

	//modify g_slot
	g_slot[start_slot+increment].chb_a=STATUS_INTER;
	g_slot[start_slot+increment].chb_b=STATUS_INTER;
	g_slot[start_slot+increment].back=size;	
	g_slot[start_slot+increment].outTime=rand()%TMO_MAX;
	//add imformation to message
	fatdma_message(start_slot+increment,size,g_message);
	//send message
	send_message(start_slot,g_slot[start_slot].chb_S,g_message);
}

void fatdma_message(unsigned short start,unsigned char slot_sum,void *message)
{
	
}

