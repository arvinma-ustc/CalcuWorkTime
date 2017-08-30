#ifndef _TURBO_H
#define _TURBO_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

typedef unsigned char turbo_type;
typedef unsigned int uint32_t;

#define SNR 3



void turbo_interleaver(uint32_t *interleaver,int data_len);
void turbo_encode(int len,turbo_type *serialout,turbo_type *input,double codeRate,uint32_t *interleave_pattern);
void max_log_map(char *soft_value,char *extern_value,char *input, char *apri,int len);



#endif





