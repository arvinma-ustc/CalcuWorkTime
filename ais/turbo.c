#include "turbo.h"



int main()
{
	int data_len = 128;//1920
	double codeRate =0.5;  // 1/2
	int i,j,k;
	int m=3;
	int lenwithteil=0;
	int iteras= 1;

	uint32_t *pattern = NULL;
	turbo_type *info = NULL;
	turbo_type *coded_bits=NULL;

	turbo_type info_old[128]= {0,1,0,0,0,1,0, 0, 1, 1, 1,0 ,0 ,1 , 1, 1, 0, 0, 0, 1, 1,1,

   0 ,0 ,0,0,1,1 ,1 ,1,1,1,1,0,1,0,1,0,0,1,0 ,1,1 ,0,

   1 ,0 ,1,1,0,0,0,0,0,1,0,1,0 ,1 ,0,1,1, 1 , 0 ,1 ,1, 1,

   0 , 0,1 , 0, 0 ,0,1,1,0,1,1,1,1,1,1,0 , 1,0 ,1 ,1,0,1,

     0,0,0,1,1,0,1,1 ,0,0,1,1,1,0,0,0  ,0 ,1 ,0  ,1, 1,0,

    1 ,1,0 ,1 , 0,1,1,1 ,0,0 ,1 ,0,0 ,1,1 ,1  ,0, 0};
	
	char *input=NULL;
	char *rsc_in1=NULL;
	char *rsc_in2=NULL;
	char *sys_info=NULL;
	char *check_info=NULL;
	char *apri=NULL;
	char *tmp=NULL;
	char *soft_value2=NULL;
	char *output=NULL;
	

	printf("\n hi turbo encode and decode\n");

	srand((unsigned)time(NULL)); //用时间做种，每次产生随机数不一样

	info = (turbo_type *)malloc(data_len*sizeof(turbo_type));
	if(info == NULL)
		{
		printf("\n  malloc momey error and exit\n");
		exit(0);
		}

	coded_bits = (turbo_type *)malloc(2*(data_len + 2*m)*sizeof(turbo_type));
	if(coded_bits == NULL)
		{
		printf("\n coded_bits malloc momey error and exit\n");
		exit(0);
		}

	input = (char *)malloc(2*(data_len + 2*m)*sizeof(char));
	if(input == NULL)
		{
		printf("\n input malloc momey error and exit\n");
		exit(0);
		}
	rsc_in1 = (char *)malloc(2*(data_len + 2*m)*sizeof(char));
	if(rsc_in1 == NULL)
		{
		printf("\n rsc_in1 malloc momey error and exit\n");
		exit(0);
		}
	rsc_in2= (char *)malloc(2*(data_len + 2*m)*sizeof(char));
	if(rsc_in2== NULL)
		{
		printf("\n rsc_in2 malloc momey error and exit\n");
		exit(0);
		}
	sys_info= (char *)malloc((data_len + 2*m)*sizeof(char));
	if(sys_info== NULL)
		{
		printf("\n sys_info malloc momey error and exit\n");
		exit(0);
		}
	check_info= (char *)malloc((data_len + 2*m)*sizeof(char));
	if(check_info== NULL)
		{
		printf("\n check_info malloc momey error and exit\n");
		exit(0);
		}

	apri = (char *)malloc((data_len + 2*m)*sizeof(char));
	if(apri== NULL)
		{
		printf("\n apri malloc momey error and exit\n");
		exit(0);
		}
	tmp  = (char *)malloc(2*(data_len + 2*m)*sizeof(char));
	if(tmp== NULL)
		{
		printf("\n tmp malloc momey error and exit\n");
		exit(0);
		}
	soft_value2 = (char *)malloc(2*(data_len + 2*m)*sizeof(char));
	if(soft_value2== NULL)
		{
		printf("\n soft_value2 malloc momey error and exit\n");
		exit(0);
		}
//pattern
	pattern = (uint32_t *)malloc(data_len*sizeof(uint32_t));
	if(pattern == NULL)
		{
		printf("\n	pattern malloc momey error and exit\n");
		exit(0);
		}

	
	output= (turbo_type *)malloc(data_len*sizeof(turbo_type));
		if(output == NULL)
			{
			printf("\n	malloc momey error and exit\n");
			exit(0);
			}
//	printf("pattern address is : %x \n", pattern);

	for(i=0;i<data_len;i++)
		{
		info[i]=rand()%2;
		//info[i]=info_old[i];
		}
	
	turbo_interleaver(pattern,data_len);
	turbo_encode(data_len,coded_bits,info,codeRate,pattern);

//   input = LLR;
for(i=0;i<2*(data_len + 2*m);i++)
{
	if(coded_bits[i]>0)
		input[i]=1;
	else
		input[i]=-1;
}
// 还原turbo码的输入

//
if(codeRate==0.5)
	{
	lenwithteil=data_len + 2*m;
	for(i=0;i<(data_len + 2*m);i++)
		{
		sys_info[i]=input[2*i];
		check_info[i]=input[2*i+1];
		}
		
	for(i=0;i<(data_len + 2*m);i++)
		{
		rsc_in1[i]=sys_info[i];
		if(i%2==0)
			rsc_in1[(data_len + 2*m)+i]=check_info[i];		
		}
	for(i=0;i<data_len;i++)
		{
		rsc_in2[i]=sys_info[pattern[i]];
		if(i%2==1)
			rsc_in2[(data_len + 2*m)+i]=check_info[i];		
		}
	for(i=data_len;i<(data_len + 2*m);i++)
		{
		rsc_in2[i]=sys_info[i];		
		}	
	}
//
if(codeRate==0.75)//nothing cause it not support in this case
{}

	for(i=0;i<(data_len + 2*m);i++)
		{
		apri[i]=0;	
		tmp[2*i]=0;
		soft_value2[2*i]=0;
		tmp[2*i+1]=0;
		soft_value2[2*i+1]=0;
		}	

/*
% 第一个rsc的译码
    [tmp,apri] = max_log_map(rsc_in1, apri);
    apri(1:end-6) = apri(interleave_pattern);
    apri(end - 5 : end) = 0;
    % 第二个rsc的译码
    [soft_value2, apri] = max_log_map(rsc_in2, apri);
    apri(interleave_pattern) = apri(1:end-6);
    apri(end - 5 : end) = 0;
    
    %% 译码结束，输出
    soft_value2(interleave_pattern) = soft_value2(1 : len);
    output = (sign(soft_value2(1 : len)) + 1) / 2;
    coded_ber = sum(abs(output - tx_bits))

*/	
	max_log_map(tmp,apri,rsc_in1, apri,data_len + 2*m);

	for(i=0;i<(data_len);i++)
		tmp[i]=apri[pattern[i]];
	for(i=0;i<(data_len);i++)
		apri[i]=tmp[i];
	for(i=data_len;i<(data_len+2*m);i++)
		apri[i]=0;

	max_log_map(soft_value2,apri,rsc_in2, apri,2*(data_len + 2*m));

	for(i=0;i<(data_len);i++)
		tmp[i]=apri[pattern[i]];
	for(i=0;i<(data_len);i++)
		apri[i]=tmp[i];
	for(i=data_len;i<(data_len+2*m);i++)
		apri[i]=0;

	for(i=0;i<(data_len);i++)
		tmp[pattern[i]]=soft_value2[i];
	for(i=0;i<(data_len);i++)
		soft_value2[i]=tmp[i];

	for(i=0;i<(data_len);i++)
		{
		if(soft_value2[i]>0)
			output[i]=1;
		else
			output[i]=0;
		}

//debug message
	for(i=0;i<128;i++)
			{
			printf("  %d   ",info[i]);
			}
	printf("\n\n\n");
	for(i=0;i<128;i++)
			{
			printf("  %d   ",output[i]);
			}
	printf("\n");
//debug
	free(info);
	free(pattern);
	pattern = NULL;
	free(coded_bits);
	coded_bits = NULL;
	free(input);
	input = NULL;
	free(rsc_in1);
	free(rsc_in2);
	free(sys_info);
	free(check_info);
	free(apri);
	free(tmp);
	free(soft_value2);
	free(output);
	
	return 0;
}



void turbo_interleaver(uint32_t *pattern,int data_len)
{  
	uint32_t k1,k2;
	uint32_t s,m,i,j,t,q,c;
	uint32_t p[8]={31,37,43,47,53,59,61,67};

	if(data_len == 23552)
		{
		k1=8;
		k2=2944;
		}
	else if(data_len == 128)
		{
		k1=2;
		k2=64;
		}
	else if(data_len == 1920)
		{
		k1=4;
		k2=480;
		}
	else if(data_len == 23056)
		{
		k1=8;
		k2=2882;
		}
	else if(data_len == 32800)
		{
		k1=10;
		k2=3280;
		}
	else if(data_len == 136)
		{
		k1=2;
		k2=68;
		}
	else if(data_len == 296)
		{
		k1=2;
		k2=148;
		}
	else
		{
		printf("arugment  datalen error\n");
		exit(0);
		}
	for(s=1;s<=data_len;s++)
		{
		m=(s-1)%2;
		i=(s-1)/(2*k2);
		j=(s-1)/2 - (i*k2);
		t=(19*i + 1)%(k1/2);
		q=t%8 + 1;
		c=((p[q-1]*j)+21*m)%k2;
		pattern[s-1]=2*(t+(c*k1)/2 +1)-m;
		}
	
}



void turbo_encode(int len,turbo_type *serialout,turbo_type *input,double codeRate,uint32_t *interleave_pattern)
{
	turbo_type g[2][4] = {1,1,0,1,1,1,1,1};
	turbo_type f[4] = {1,0,1,1}; 
	turbo_type state[3] = {0,0,0}; 
	int m=3;
	int k;
	int rsc_in,i,tmp,cnt;

	turbo_type *output=NULL;
	turbo_type *interleave_data=NULL;

	output = (turbo_type *)malloc(6*(len + 2*m)*sizeof(turbo_type));
	if(output == NULL)
		{
		printf("\n output malloc momey error and exit\n");
		exit(0);
		}
	interleave_data = (turbo_type *)malloc(len*sizeof(turbo_type));
	if(interleave_data == NULL)
		{
		printf("\n interleave_data malloc momey error and exit\n");
		exit(0);
		}
//encode one	
	for(k=1;k<=(len + 2*m);k++)
		{
		if(k<=len)
			{
			rsc_in = input[k-1]*f[0];
			for(i=0;i<m;i++)
				rsc_in += state[i]*f[i+1];
			rsc_in = rsc_in%2;
			output[k-1]=input[k-1];
			}
		else
			{
			 rsc_in =0; 
	      output[k-1] = 0;
			}
//output(2, k) = rem([rsc_in state] * g(1, :).', 2);
		tmp=rsc_in*g[0][0];
		for(i=0;i<m;i++)
			tmp += state[i]*g[0][i+1];
		output[(len+2*m) + k-1]=tmp%2;
//output(3, k) = rem([rsc_in state] * g(2, :).', 2);
		tmp=rsc_in*g[1][0];
		for(i=0;i<m;i++)
			tmp += state[i]*g[1][i+1];
		output[2*(len+2*m) + k-1]=tmp%2;

		state[2]=state[1];
		state[1]=state[0];
		state[0]=rsc_in;

		}
//interleave_data = input(interleave_pattern);
	for(i=0;i<len;i++)
		interleave_data[i]=input[interleave_pattern[i]-1];
	
//encode two

for(k=1;k<=(len + 2*m);k++)
	{
	if(k<=len)
		{
		rsc_in = interleave_data[k-1]*f[0];
		for(i=0;i<m;i++)
			rsc_in += state[i]*f[i+1];
		rsc_in = rsc_in%2;
		output[3*(len+2*m) + k-1]=interleave_data[k-1];
		}
	else
		{
		 rsc_in =0; 
      output[3*(len+2*m) + k-1] = 0;
		}
//output(2, k) = rem([rsc_in state] * g(1, :).', 2);
	tmp=rsc_in*g[0][0];
	for(i=0;i<m;i++)
		tmp += state[i]*g[0][i+1];
	output[4*(len+2*m) + k-1]=tmp%2;
//output(3, k) = rem([rsc_in state] * g(2, :).', 2);
	tmp=rsc_in*g[1][0];
	for(i=0;i<m;i++)
		tmp += state[i]*g[1][i+1];
	output[5*(len+2*m) + k-1]=tmp%2;


		state[2]=state[1];
		state[1]=state[0];
		state[0]=rsc_in;

		}	


//delete 
if(codeRate==0.5)
{
	for(i=1;i<=(len + 2*m);i++)
	{
	if(i<=len)
		{
		if(i%2==1)
			{
			serialout[2*i-1-1]=output[i-1];
			serialout[2*i-1]=output[(len+2*m) + i-1];
			}
		else
			{
			serialout[2*i-1-1]=output[i-1];
			serialout[2*i-1]=output[4*(len+2*m) + i-1];
			}
		}
	else
		{
		if(i<=len+m)
			{
			serialout[2*i-1-1]=output[i-1];
			serialout[2*i-1]=output[(len+2*m) + i-1];
			}
		else
			{
			serialout[2*i-1-1]=output[3*(len+2*m) + i-1];
			serialout[2*i-1]=output[4*(len+2*m) + i-1];
			} 
		}
	}
}

if(codeRate==0.75)
{
	cnt=0;
	for(i=1;i<=(len + 2*m);i++)
	{
	if(i<=len)
		{
		if(i%6==1)
			{
			serialout[cnt]=output[i-1];
			serialout[cnt+1]=output[2*(len+2*m) + i-1];
			cnt+=2;
			}
		else if(i%6==0)
			{
			serialout[cnt]=output[i-1];
			serialout[cnt+1]=output[5*(len+2*m) + i-1];
			cnt+=2;
			}
		else
			{
			serialout[cnt]=output[i-1];
			cnt+=1;
			}
		}
	else
		{
		if(i<=len+m)
			{
			serialout[cnt]=output[i-1];
			serialout[cnt+1]=output[2*(len+2*m) + i-1];
			cnt+=2;
			}
		else
			{
			serialout[cnt]=output[3*(len+2*m) + i-1];
			serialout[cnt+1]=output[5*(len+2*m) + i-1];
			cnt+=2;
			} 
		}
	}
}


	free(output);
	output = NULL;
	free(interleave_data);
	interleave_data = NULL;
}


void max_log_map(char *soft_value,char *extern_value,char *input, char *apri,int len)
{
/* ===========状态转移图开始===========
	... S(i)  0-S(i+1)/b(i)b(i+1)  1-S(i+1)/b(i)b(i+1)
	... 1			1/00				5/11 
	... 2			5/00				1/11
	... 3			6/01				2/10
	... 4			2/01				6/10
	... 5			3/01				7/10
	... 6			7/01				3/10
	... 7			8/00				4/11
	... 8			4/00				8/11
	... ===========状态转移图结束===========
	
 初始化*/

int infty = 128;
int state = 8;	//2^3
int i=0,k=0,temp1=0,temp2=0,temp=0;

char next_state[] = {	1,5,6,2,3,7,8,4,
					5,1,2,6,7,3,4,8};
char last_state[] = {1,4,5,8,2,3,6,7,
             2,3,6,7,1,4,5,8};


char *x=NULL;
char *y=NULL;
int *alpha=NULL;
int *beta=NULL;
int *gamma=NULL;

	

	x= (char *)malloc((len)*sizeof(char));
	if(x == NULL)
		{
		printf("\n x malloc momey error and exit\n");
		exit(0);
		}
	y= (char *)malloc((len)*sizeof(char));
	if(y== NULL)
		{
		printf("\n y malloc momey error and exit\n");
		exit(0);
		}
	alpha= (int *)malloc((len*state)*sizeof(int));
	if(alpha == NULL)
		{
		printf("\n alpha malloc momey error and exit\n");
		exit(0);
		}
	beta= (int *)malloc((len+1)*state*sizeof(int));
	if(beta== NULL)
		{
		printf("\n beta malloc momey error and exit\n");
		exit(0);
		}
//	gamma= (int *)malloc(state*2*len*sizeof(int));
	gamma= (int *)malloc(16*len*sizeof(int));
	if(gamma== NULL)
		{
		printf("\n beta malloc momey error and exit\n");
		exit(0);
		}

	for(i=0;i<len;i++)
		x[i]=input[i];
	for(i=0;i<len;i++)
		y[i]=input[i+len];

/*
alpha = - infty * ones(state, len);
alpha(1, 1) = 0;
beta = - infty * ones(state, len + 1);
beta(1, len + 1) = 0;
gamma = zeros(state, 2, len);
gamma([3, 4, 5, 6], 1, :) = repmat(y, 4, 1);
gamma([3, 4, 5, 6], 2, :) = repmat(x + apri, 4, 1);
gamma([1, 2, 7, 8], 2, :) = repmat(x + y + apri, 4, 1);
*/	
	for(i=0;i<len*state;i++)
		alpha[i]=-infty;
	alpha[0]=0;
	
	for(i=0;i<len*state;i++)
		beta[i]=-infty;
	beta[len]=0;
	
	for(i=0;i<len*state*2;i++)
		gamma[i]=0;


	for(i=0;i<len;i++)
		{
		gamma[2*2*len+i]=y[i];
		gamma[3*2*len+i]=y[i];
		gamma[4*2*len+i]=y[i];
		gamma[5*2*len+i]=y[i];

		gamma[2*2*len+len+i]=x[i]+apri[i];
		gamma[3*2*len+len+i]=x[i]+apri[i];
		gamma[4*2*len+len+i]=x[i]+apri[i];
		gamma[5*2*len+len+i]=x[i]+apri[i];

		gamma[len+i]=x[i]+apri[i]+y[i];
		gamma[1*2*len+len+i]=x[i]+apri[i]+y[i];
		gamma[6*2*len+len+i]=x[i]+apri[i]+y[i];
		gamma[7*2*len+len+i]=x[i]+apri[i]+y[i];
	//	*/
		}

printf("\n here is ok \n");

	for(k=1;k<len;k++)
		{
		for(i=0;i<state;i++)
			{
			temp1 = gamma[2*len*(last_state[i]-1)+k-1]^2 +alpha[len*(last_state[i]-1)+k-1];
			temp2 = gamma[2*len*(last_state[state+i]-1)+len+k-1]^2 +alpha[len*(last_state[state+i]-1)+k-1];
			if(temp1 > temp2)
				alpha[i*len+k]=temp1;
			else
				alpha[i*len+k]=temp2;
			}
		}
	/*%% 计算分支度量beta和LLR
	for k = len : -1 : 1
		beta(:, k) = max(squeeze(gamma(:, 1, k)) + beta(next_state(1, :), k + 1), ...
						 squeeze(gamma(:, 2, k)) + beta(next_state(2, :), k + 1));
		soft_value(k) = max(alpha(:, k) + squeeze(gamma(:, 2, k)) + beta(next_state(2, :), k + 1)) - ...
						max(alpha(:, k) + squeeze(gamma(:, 1, k)) + beta(next_state(1, :), k + 1)) ;
	end*/

	for(k=len-1;k>=0;k--)
		{
		for(i=0;i<state;i++)
			{
			temp1 = gamma[2*len*i+k]^2 +beta[len*(next_state[i]-1)+k+1];
			temp2 = gamma[2*len*i+len+k]^2 +beta[len*(next_state[state+i]-1)+k+1];
			if(temp1 > temp2)
				beta[i*len+k]=temp1;
			else
				beta[i*len+k]=temp2;
			}
		temp1=0;
		temp2=0;
		for(i=0;i<state;i++)
			{
			temp=alpha[i*len+k]+ gamma[2*len*i+len+k]^2+beta[len*(next_state[state+i]-1)+k+1];
			if(temp>temp1)
				temp1=temp;
			temp=alpha[i*len+k]+ gamma[2*len*i+k]^2 +beta[len*(next_state[i]-1)+k+1];
			if(temp>temp2)
				temp2=temp;
			}
		soft_value[k]=temp1 - temp2;
		}

	for(k=0;k<len;k++)
		extern_value[k]=soft_value[k]-x[i]-apri[i];
	free(x);
	free(y);
	free(alpha);
	free(beta);
	free(gamma);
}






