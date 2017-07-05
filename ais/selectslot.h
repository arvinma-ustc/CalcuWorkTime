#ifndef _SELECTSLOT_H
#define _SELECTSLOT_H

#define STATUS_FREE 	1
#define STATUS_INTER 	2
#define STATUS_EXTER 	3
#define STATUS_BASE 	4
#define STATUS_TIME		5
#define STATUS_DISTAN	6
#define STATUS_X		7

#define SLOT_TOTLE		2250
#define SI_start_MIN	0
#define SI_start_MAX	2249
#define Rr_MIN			2
#define Rr_MAX			30
#define SLOT_LEN_MIN	1
#define SLOT_LEN_MAX	5
#define SLOT_CANDI_MAX	4

#define CHANNEL_A		75
#define CHANNEL_B 		76

#define debug_mode 0

struct select_interval_struct
{
	unsigned int start;
	unsigned int end;
	unsigned int nominal_slot;
	unsigned int report_index;
	unsigned int report_rate;
};

struct slot_candidate_struct
{
	int slot_index[SLOT_CANDI_MAX];
	unsigned int cadidate_sum;
	unsigned int slot_length;
	unsigned int report_index;
	unsigned int report_rate;
};

void Choice_Slot(unsigned int SI_start,unsigned int Rr,	unsigned int length);

void Get_Candidateslot_from_Interval(struct select_interval_struct *interval,
	struct slot_candidate_struct *candidate,int channel);
void Get_Candidate_Avalu(struct select_interval_struct *interval,
	struct slot_candidate_struct *candidate,char *buffer,int channel);

void status_init();
int Selcet_Slot_from_Candidate(struct slot_candidate_struct *candidate);
void Slot_Free(struct select_interval_struct *interval,char *buffer);
void ReuseRule_One(struct select_interval_struct *interval,char *buffer);
void ReuseRule_Two(struct select_interval_struct *interval,char *buffer);
void ReuseRule_Three(struct select_interval_struct *interval,char *buffer);
void ReuseRule_Four(struct select_interval_struct *interval,char *buffer);
void ReuseRule_Five(struct select_interval_struct *interval,char *buffer);
void Reuse_Internally(struct select_interval_struct *interval,char *buffer);

#endif
