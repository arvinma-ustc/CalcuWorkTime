#include "selectslot.h"
#include "AIS_TDMA.h"
#include<stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include "main.h"



/*ȫ�ֱ���������main.c��*/
extern struct SLOT_S g_slot[MAX_SLOT_COUNT];
extern unsigned short int g_slot_index;


unsigned int slot_tran[30];


/*left 4 bits for channel A,right 4 bits for channel B */
/*���������ʱ϶֡ͼ��SI_START��Rr����Ϣ����(length)��ʱ϶
֡ͼ���зָ��Rr�� select internally, ����Э����slot ѡȡ������
select internally ��ÿ��slot�Ƿ������Ϊ��ѡslot����������洢��available
 buffer �У�Ȼ���ٴӱ��Ϊ���õ�slot��ѡ��ѡslot������ҵ�
 ��4����ѡslot�������һ���ӱ�ѡslot��ѡ��һ��slotʹ�ã������
 ����4����ʹ��slot���ù��򽫸����slot
���Ϊ���ã����½���Ѱ�ұ�ѡslot����*/

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

/*�������Ĳ����Ƿ�Ϸ�*/	
	if(SI_start < SI_start_MIN ||SI_start> SI_start_MAX ||
		Rr < Rr_MIN || Rr > Rr_MAX ||
		length < SLOT_LEN_MIN || length > SLOT_LEN_MAX)
	{
		printf("parament error\n");
		exit(1);
	}
/*����һ��buffer�洢ѡ��Ľ��,Rr��int��slot���*/
	buffer = (int *)malloc(Rr*sizeof(int));
	if(buffer==NULL)
	{
		printf("malloc momery for buffer fail\n");
		exit(1);
	}
/*ÿ��select internally�����select internally�����Ǽ����0.2��*/
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
/*avaluble buffer���һ��SI��ÿ��slot�Ƿ������Ϊ��ѡslot*/
	avaluble_buffer = (char *)malloc((interval->end - interval->start)*sizeof(char));
	if(avaluble_buffer==NULL)
	{
		printf("malloc momery for avaluble_buffer fail\n");
		exit(1);
	}
	memset(avaluble_buffer,'\0', sizeof(avaluble_buffer));
/*����Э�����slot���ȱ��Ϊ����Ϊ��ѡslot*/	
	Slot_Free(interval, avaluble_buffer);
/*���ݱ�ǵ����ѡȡ��ѡslot*/	
	Get_Candidate_Avalu(interval,candidate,avaluble_buffer,channel);
/*���ѡȡ�ı�ѡslot�ﲻ��4����ʹ��slot reuse�����Ǹ����slotΪ���ñ�ѡ
slot ������ѡ��ѡslot��slot reuse��5������Ҫ���ʹ��*/
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
/*������к����ù�����Ӧ֮���޷�ѡȡ��1����ѡslot������
��վ�Ѿ�ռ�е�slot*/
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
/*��һ��selcet inerval �еĿ���ʱ϶���Ϊ����Ϊ��ѡʱ϶���洢��buffer��*/
void Slot_Free(struct select_interval_struct *interval,char *buffer)
{
	unsigned int slot_index;
	unsigned int buffer_index=0;
	int tempindex=0;
	
	for(slot_index=interval->start;slot_index<interval->end;slot_index++)
	{
		
	if(g_slot[slot_index].chb_b==STATUS_FREE &&
		g_slot[slot_index].chb_a==STATUS_FREE)
	{/*�����������ʱ϶����һ�ŵ����Ƿ�Ϊ�ڲ����֣�������򲻷���
	�ŵ��л�Ҫ�󣬲�����Ϊ���õĺ�ѡʱ϶���б��*/
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

/*����ʱ϶���ù���һ��һ��selcet inerval ������������ʱ϶���Ϊ����Ϊ
��ѡʱ϶���洢��buffer��*/


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

/*����ʱ϶���ù����һ��selcet inerval ������������ʱ϶���Ϊ����Ϊ
��ѡʱ϶���洢��buffer��*/

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

/*����ʱ϶���ù�������һ��selcet inerval ������������ʱ϶���Ϊ����Ϊ
��ѡʱ϶���洢��buffer��*/

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

/*����ʱ϶���ù����Ľ�һ��selcet inerval ������������ʱ϶���Ϊ����Ϊ
��ѡʱ϶���洢��buffer��*/

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


/*����ʱ϶���ù����彫һ��selcet inerval ������������ʱ϶���Ϊ����Ϊ
��ѡʱ϶���洢��buffer��*/

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

/*�������ñ�վ̨�ڲ�����ʱ϶��һ��selcet inerval ������������ʱ϶���Ϊ����Ϊ
��ѡʱ϶���洢��buffer��*/

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
/*����buffer��ʱ϶�Ƿ����Ϊ��ѡʱ϶�ı�ǣ�ѡ������candidate->length����
��ʱ϶��Ϊ��ѡʱ϶*/
void Get_Candidate_Avalu(struct select_interval_struct *interval,
	struct slot_candidate_struct *candidate,char *buffer,int channel)
{
	unsigned int slot_index;
	unsigned int index_dup;
	unsigned int length_index;
	for(slot_index=interval->start;slot_index<(interval->end - candidate->slot_length);slot_index++)
	{
		index_dup =slot_index - interval->start;
		/*Ѱ������candidate->length������Ϊ��ѡʱ϶������ʱ϶��������
		��һ��ʱ϶��Ϊ��ѡʱ϶*/
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

/*�Ӻ�ѡʱ϶�����ѡȡһ��ʱ϶��Ϊ����ʹ�õ�ʱ϶*/
int Selcet_Slot_from_Candidate(struct slot_candidate_struct *candidate)
{
	if(candidate->cadidate_sum<1)
		return -1;
	else
		return candidate->slot_index[(rand()%candidate->cadidate_sum)];
}

