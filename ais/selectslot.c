#include "selectslot.h"
#include "AIS_TDMA.h"
#include<stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include "main.h"



/*全局变量定义在main.c中*/
extern struct SLOT_S g_slot[MAX_SLOT_COUNT];
extern unsigned short int g_slot_index;


unsigned int slot_tran[30];


/*left 4 bits for channel A,right 4 bits for channel B */
/*根据输入的时隙帧图，SI_START，Rr，消息长度(length)对时隙
帧图进行分割成Rr个 select internally, 根据协议中slot 选取规则标记
select internally 中每个slot是否可以作为备选slot，并将结果存储在available
 buffer 中，然后再从标记为可用的slot中选择备选slot，如果找到
 了4个备选slot则进入下一步从备选slot中选择一个slot使用，如果找
 不到4个则使用slot重用规则将更多的slot
标记为可用，重新进入寻找备选slot步骤*/

void status_init()
{
	int size;

	for(size=0;size<2250;size+=5)
	{
		g_slot[size].chb_a = 1;
		g_slot[size+1].chb_a = 1;
		g_slot[size+2].chb_a = 4;
		g_slot[size+3].chb_a = 5;		
		g_slot[size+4].chb_a = 2;
		g_slot[size].chb_b= 1;
		g_slot[size+1].chb_b = 1;
		g_slot[size+2].chb_b = 4;
		g_slot[size+3].chb_b = 5;		
		g_slot[size+4].chb_b = 2;
	}

}



void Choice_Slot(unsigned int SI_start,unsigned int Rr,
				unsigned int length)
{
	int Rr_index=0;
	int channel=0;
	struct select_interval_struct interval;
	struct slot_candidate_struct candidate;
	unsigned int naminal_incre;
	int *buffer;

/*检查输入的参数是否合法*/	
	if(SI_start < SI_start_MIN ||SI_start> SI_start_MAX ||
		Rr < Rr_MIN || Rr > Rr_MAX ||
		length < SLOT_LEN_MIN || length > SLOT_LEN_MAX)
	{
		printf("parament error\n");
		exit(1);
	}
/*申请一段buffer存储选择的结果,Rr个int型slot编号*/
	buffer = (int *)malloc(Rr*sizeof(int));
	if(buffer==NULL)
	{
		printf("malloc momery for buffer fail\n");
		exit(1);
	}
/*每个select internally间隔，select internally长度是间隔的0.2倍*/
	naminal_incre = SLOT_TOTLE/Rr;

	for(Rr_index=0;Rr_index < Rr;Rr_index++)
	{
		interval.start=SI_start+naminal_incre*Rr_index;
		interval.end = interval.start + 2*naminal_incre/10;
		interval.nominal_slot = interval.start + naminal_incre/10;
		interval.report_index = Rr_index;
		interval.report_rate = Rr;
		candidate.slot_length = length;
		candidate.report_index = Rr_index;
		candidate.report_rate = Rr;
		candidate.cadidate_sum = 0;
		if(channel==CHANNEL_A)
			channel = CHANNEL_B;
		else
			channel = CHANNEL_A;

		Get_Candidateslot_from_Interval(&interval,&candidate,channel);
		buffer[Rr_index]=Selcet_Slot_from_Candidate(&candidate);
	}
	printf("\n");
	for(Rr_index=0;Rr_index < Rr;Rr_index++)
	{
		printf("%d		",buffer[Rr_index]);
	}
	printf("\n");
}

void Get_Candidateslot_from_Interval(struct select_interval_struct *interval,
	struct slot_candidate_struct *candidate,int channel)
{
	
	char *avaluble_buffer;
/*avaluble buffer标记一个SI中每个slot是否可以作为备选slot*/
	avaluble_buffer = (char *)malloc((interval->end - interval->start)*sizeof(char));
	if(avaluble_buffer==NULL)
	{
		printf("malloc momery for avaluble_buffer fail\n");
		exit(1);
	}
	memset(avaluble_buffer,'\0', sizeof(avaluble_buffer));
/*根据协议空闲slot首先标记为可用为备选slot*/	
	Slot_Free(interval, avaluble_buffer);
/*根据标记的情况选取备选slot*/	
	Get_Candidate_Avalu(interval,candidate,avaluble_buffer,channel);
/*如果选取的备选slot达不到4个则使用slot reuse规则标记更多的slot为可用备选
slot 再重新选择备选slot，slot reuse有5个规则要逐个使用*/
	if(candidate->cadidate_sum < SLOT_CANDI_MAX)
	{
		candidate->cadidate_sum = 0;
		ReuseRule_One(interval, avaluble_buffer);
		Get_Candidate_Avalu(interval,candidate,avaluble_buffer,channel);
	}
	if(candidate->cadidate_sum < SLOT_CANDI_MAX)
	{
		candidate->cadidate_sum = 0;
		ReuseRule_Two(interval, avaluble_buffer);
		Get_Candidate_Avalu(interval,candidate,avaluble_buffer,channel);
	}
	if(candidate->cadidate_sum < SLOT_CANDI_MAX)
	{
		candidate->cadidate_sum = 0;
		ReuseRule_Three(interval, avaluble_buffer);
		Get_Candidate_Avalu(interval,candidate,avaluble_buffer,channel);
	}
	if(candidate->cadidate_sum < SLOT_CANDI_MAX)
	{
		candidate->cadidate_sum = 0;
		ReuseRule_Four(interval, avaluble_buffer);
		Get_Candidate_Avalu(interval,candidate,avaluble_buffer,channel);
	}
	if(candidate->cadidate_sum < SLOT_CANDI_MAX)
	{
		candidate->cadidate_sum = 0;
		ReuseRule_Five(interval, avaluble_buffer);
		Get_Candidate_Avalu(interval,candidate,avaluble_buffer,channel);
	}
/*如果空闲和重用规则适应之后都无法选取到1个备选slot则重用
本站已经占有的slot*/
	if(candidate->cadidate_sum < 1)
	{
		candidate->cadidate_sum = 0;
		Reuse_Internally(interval, avaluble_buffer);
		Get_Candidate_Avalu(interval,candidate,avaluble_buffer,channel);
	}

	if(candidate->cadidate_sum < 1)
	{
		printf("can`t find any slot for candidate\n");
		exit(1);
	}
	free(avaluble_buffer);
}
/*将一个selcet inerval 中的空闲时隙标记为可做为候选时隙，存储在buffer中*/
void Slot_Free(struct select_interval_struct *interval,char *buffer)
{
	unsigned int slot_index;
	unsigned int buffer_index=0;
	int tempindex=0;
	
	for(slot_index=interval->start;slot_index<interval->end;slot_index++)
	{
		
	if(g_slot[slot_index].chb_b==STATUS_FREE &&
		g_slot[slot_index].chb_a==STATUS_FREE)
	{/*检测左右相邻时隙在另一信道中是否为内部划分，如果是则不符合
	信道切换要求，不能作为可用的候选时隙进行标记*/
		if(g_slot[(slot_index + 2249)%MAX_SLOT_COUNT].chb_b!=STATUS_INTER && g_slot[(slot_index + 1)%MAX_SLOT_COUNT].chb_b!=STATUS_INTER)
			buffer[buffer_index]+=0x10;
		if(g_slot[(slot_index + 2249)%MAX_SLOT_COUNT].chb_a!=STATUS_INTER && g_slot[(slot_index + 1)%MAX_SLOT_COUNT].chb_a!=STATUS_INTER)
			buffer[buffer_index]+=0x01;
	}

	buffer_index++;
	}
#if debug_mode
	printf("\n");
		printf("interval start is: %d\n",interval->start);

		printf("\n avalube buffer\n");
		for(buffer_index=interval->start;buffer_index<interval->end;buffer_index++)
			printf("%x ",buffer[tempindex++]);
		
		printf("\n");
#endif

		
}

/*根据时隙重用规则一将一个selcet inerval 中满足条件的时隙标记为可做为
候选时隙，存储在buffer中*/


void ReuseRule_One(struct select_interval_struct *interval,char *buffer)
{
	unsigned int slot_index;
	unsigned int buffer_index=0;
	for(slot_index=interval->start;slot_index<interval->end;slot_index++)
	{		
	if(g_slot[slot_index].chb_b ==STATUS_FREE &&
		((g_slot[slot_index].chb_a ==STATUS_DISTAN)||(g_slot[slot_index].chb_a==STATUS_TIME)))				
	{
		if(g_slot[(slot_index + 2249)%SLOT_TOTLE].chb_a!=STATUS_INTER && g_slot[(slot_index + 1)%SLOT_TOTLE].chb_a!=STATUS_INTER)
			buffer[buffer_index]+=0x01;
	}
	if(((g_slot[slot_index].chb_b==STATUS_DISTAN)||(g_slot[slot_index].chb_b==STATUS_TIME)) &&
		g_slot[slot_index].chb_a ==STATUS_FREE)				
	{
		if(g_slot[(slot_index + 2249)%SLOT_TOTLE].chb_b!=STATUS_INTER && g_slot[(slot_index + 1)%SLOT_TOTLE].chb_b!=STATUS_INTER)
			buffer[buffer_index]+=0x10;
	}
	buffer_index++;
	}	
}

/*根据时隙重用规二将一个selcet inerval 中满足条件的时隙标记为可做为
候选时隙，存储在buffer中*/

void ReuseRule_Two(struct select_interval_struct *interval,char *buffer)
{
	unsigned int slot_index;
	unsigned int buffer_index=0;
	for(slot_index=interval->start;slot_index<interval->end;slot_index++)
	{		
	if(g_slot[slot_index].chb_b ==STATUS_FREE &&
		((g_slot[slot_index].chb_a ==STATUS_DISTAN)||(g_slot[slot_index].chb_a==STATUS_TIME)))				
	{
		if(g_slot[(slot_index + 2249)%SLOT_TOTLE].chb_b!=STATUS_INTER && g_slot[(slot_index + 1)%SLOT_TOTLE].chb_b!=STATUS_INTER)
			buffer[buffer_index]+=0x10;
	}
	if(((g_slot[slot_index].chb_b ==STATUS_DISTAN)||(g_slot[slot_index].chb_b ==STATUS_TIME)) &&
		g_slot[slot_index].chb_a==STATUS_FREE)				
	{
		if(g_slot[(slot_index + 2249)%SLOT_TOTLE].chb_a !=STATUS_INTER && g_slot[(slot_index + 1)%SLOT_TOTLE].chb_a!=STATUS_INTER)
			buffer[buffer_index]+=0x01;
	}
	buffer_index++;
	}	
}

/*根据时隙重用规则三将一个selcet inerval 中满足条件的时隙标记为可做为
候选时隙，存储在buffer中*/

void ReuseRule_Three(struct select_interval_struct *interval,char *buffer)
{
	unsigned int slot_index;
	unsigned int buffer_index=0;
	for(slot_index=interval->start;slot_index<interval->end;slot_index++)
	{		
	if(((g_slot[slot_index].chb_b==STATUS_DISTAN)||(g_slot[slot_index].chb_b==STATUS_TIME)) &&
		((g_slot[slot_index].chb_a==STATUS_DISTAN)||(g_slot[slot_index].chb_a==STATUS_TIME)))				
	{
		if(g_slot[(slot_index + 2249)%SLOT_TOTLE].chb_b!=STATUS_INTER && g_slot[(slot_index + 1)%SLOT_TOTLE].chb_b!=STATUS_INTER)
			buffer[buffer_index]+=0x10;
		if(g_slot[(slot_index + 2249)%SLOT_TOTLE].chb_a!=STATUS_INTER && g_slot[(slot_index + 2249)%SLOT_TOTLE].chb_a!=STATUS_INTER)
			buffer[buffer_index]+=0x01;
	}
	buffer_index++;
	}	
}

/*根据时隙重用规则四将一个selcet inerval 中满足条件的时隙标记为可做为
候选时隙，存储在buffer中*/

void ReuseRule_Four(struct select_interval_struct *interval,char *buffer)
{
	unsigned int slot_index;
	unsigned int buffer_index=0;
	for(slot_index=interval->start;slot_index<interval->end;slot_index++)
	{		
	if(g_slot[slot_index].chb_b==STATUS_FREE &&
		g_slot[slot_index].chb_a ==STATUS_BASE)				
	{
		if(g_slot[(slot_index + 2249)%SLOT_TOTLE].chb_a !=STATUS_INTER && g_slot[(slot_index + 1)%SLOT_TOTLE].chb_b!=STATUS_INTER)
			buffer[buffer_index]+=0x01;
	}
	if(g_slot[slot_index].chb_b ==STATUS_BASE &&
		g_slot[slot_index].chb_a ==STATUS_FREE)				
	{
		if(g_slot[(slot_index + 2249)%SLOT_TOTLE].chb_b!=STATUS_INTER && g_slot[(slot_index + 1)%SLOT_TOTLE].chb_b!=STATUS_INTER)
			buffer[buffer_index]+=0x10;
	}
	buffer_index++;
	}	
}


/*根据时隙重用规则五将一个selcet inerval 中满足条件的时隙标记为可做为
候选时隙，存储在buffer中*/

void ReuseRule_Five(struct select_interval_struct *interval,char *buffer)
{
	unsigned int slot_index;
	unsigned int buffer_index=0;
	for(slot_index=interval->start;slot_index<interval->end;slot_index++)
	{		
	if(((g_slot[slot_index].chb_b ==STATUS_DISTAN)||(g_slot[slot_index].chb_b ==STATUS_TIME)) &&
		g_slot[slot_index].chb_a==STATUS_BASE)				
	{
		if(g_slot[(slot_index + 2249)%SLOT_TOTLE].chb_b!=STATUS_INTER && g_slot[(slot_index + 1)%SLOT_TOTLE].chb_b!=STATUS_INTER)
			buffer[buffer_index]+=0x01;
	}
	if(g_slot[slot_index].chb_b ==STATUS_BASE &&
		((g_slot[slot_index].chb_a==STATUS_DISTAN)||(g_slot[slot_index].chb_a==STATUS_TIME)))				
	{
		if(g_slot[(slot_index + 2249)%SLOT_TOTLE].chb_a!=STATUS_INTER && g_slot[(slot_index + 1)%SLOT_TOTLE].chb_b!=STATUS_INTER)
			buffer[buffer_index]+=0x10;
	}
	buffer_index++;
	}	
}

/*根据重用本站台内部划分时隙将一个selcet inerval 中满足条件的时隙标记为可做为
候选时隙，存储在buffer中*/

void Reuse_Internally(struct select_interval_struct *interval,char *buffer)
{
	unsigned int slot_index;
	unsigned int buffer_index=0;
	for(slot_index=interval->start;slot_index<interval->end;slot_index++)
	{		
		if((g_slot[slot_index].chb_b ==STATUS_INTER))				
			buffer[buffer_index]+=0x01;
		if(g_slot[slot_index].chb_a==STATUS_INTER)				
			buffer[buffer_index]+=0x10;
		buffer_index++;
	}	
}
/*根据buffer中时隙是否可用为候选时隙的标记，选择连续candidate->length长度
的时隙作为候选时隙*/
void Get_Candidate_Avalu(struct select_interval_struct *interval,
	struct slot_candidate_struct *candidate,char *buffer,int channel)
{
	unsigned int slot_index;
	unsigned int index_dup;
	unsigned int length_index;
	for(slot_index=interval->start;slot_index<(interval->end - candidate->slot_length);slot_index++)
	{
		index_dup =slot_index - interval->start;
		/*寻找连续candidate->length个可作为备选时隙的连续时隙区，并把
		第一个时隙作为候选时隙*/
		for(length_index=0;length_index<candidate->slot_length;length_index++)
		{
			if(((buffer[index_dup]&0x0f)>0 && (CHANNEL_B==channel))||
				(((buffer[index_dup]>>4)&0x0f)>0 && CHANNEL_A==channel))
				index_dup++;
			else
				break;
		}
		if(length_index==candidate->slot_length)
		{
			candidate->slot_index[candidate->cadidate_sum]=slot_index;
			candidate->cadidate_sum++;
		}
		if(candidate->cadidate_sum == 4)
			break;
	}
#if debug_mode
	printf("candidate slot sum is :%d\n",candidate->cadidate_sum);
#endif
}

/*从候选时隙中随机选取一个时隙作为最终使用的时隙*/
int Selcet_Slot_from_Candidate(struct slot_candidate_struct *candidate)
{
	if(candidate->cadidate_sum<1)
		return -1;
	else
		return candidate->slot_index[(rand()%candidate->cadidate_sum)];
}

