#include "main.h"
#include "selectslot.h"
#include "AIS_TDMA.h"


struct SLOT_S g_slot[MAX_SLOT_COUNT];
unsigned short int g_slot_index;
void *g_message = NULL;
unsigned char g_cur_Rr=0;



int main(int argc, char *argv[])
{	
	char framestatus[SLOT_TOTLE];	
	struct A_BSO_B_MESSAGE TDMA_Message;
	int si_start,Rr,length;

	g_message = (void *)&TDMA_Message;
	status_init();

	g_slot_index=50;//�����ʼ����ǰʱ϶ֵ
	Ra_TDMA_main();
/*	
	printf("hello ais\n");
//�������Ĳ�������	
	if(argc != 4)
	{	
		printf("parament number error\n");
		printf("use like: selectslot si_start Rr slot_length\n");
		exit(0);
	}

	si_start = atoi(argv[1]);	
	Rr= atoi(argv[2]);
	length= atoi(argv[3]);
	
	status_init(framestatus);//��֡״̬ͼ���г�ʼ��������ʹ��
	Choice_Slot(si_start,Rr,length);
	printf("hello ais check!\n");	
*/
}

