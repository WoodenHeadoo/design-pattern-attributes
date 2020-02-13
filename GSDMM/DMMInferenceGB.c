#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "IO.h"
#include "Iterate.h"
#include "DMMInferenceGB.h"
#include "globals.h"

extern int K,T,D,V,MaxLen;
extern double alpha,beta;
extern int * Words,* Orders;
extern int * Lengths;
extern int * Labels;
extern int * WordCounts,* SumWordCounts;
extern int * DocCounts;
extern int * Topics;
extern double * Prob;
extern int * MeanWordCounts,* MeanDocCounts;
int total;

void TopicVote(int * Temp,int duration)
{
	int i,j,t,topic;
	for(i = 0;i < D;i++)
	{
		for(j = 0;j < T;j++)
		{
			*(Temp+i*T+j) = 0;
		}
	}
	for(t = 0;t < duration;t++)
	{
		printf("%d\n",t+1);
		SampleForDocs();
		for(i = 0;i < D;i++)
		{
			topic = *(Topics+i);
			*(Temp+i*T+topic) += 1;
		}
	}
	for(i = 0;i < D;i++)
	{
		topic = 0;
		for(j = 1;j < T;j++)
		{
			if(*(Temp+i*T+j) > *(Temp+i*T+topic))
			{
				topic = j;
			}
		}
		*(Topics+i) = topic;
	}
	return;
}

int TopicNums(int * Temp)
{
	int i,j,topic,nums;
	for(j = 0;j < T;j++)
	{
		*(Temp+j) = 0;
	}
	for(i = 0;i < D;i++)
	{
		topic = *(Topics+i);
		*(Temp+topic) = 1;
	}
	nums = 0;
	for(j = 0;j < T;j++)
	{
		nums += *(Temp+j);
	}
	return nums;
}

double Perplexity()
{
	int i,k,v,topic;
	double per,temp;
	
	per = 0.0;
	for(i = 0;i < D;i++)
	{
		topic = *(Topics+i);
		temp = 0.0;
		for(k = 0;k < *(Lengths+i);k++)
		{
			v = *(Words+i*MaxLen+k);	
			temp += log((*(SumWordCounts+topic) + V*beta)/(*(WordCounts+topic*V+v) + beta));
		}
		per += temp/total;
	}
	per = exp(per);
	return per;
}

void DMMInferenceGB(double param_alpha,double param_beta,int topic_num,int iter_num,int duration)
{
	int i,j,k,t,v,dis,topic;
	int *Temp,*tTemp;
	double p,likeli;
	FILE * fp;
	T = topic_num;
	alpha = param_alpha;
	beta = param_beta;
	GetDataScale();
	
	Words = (int*)malloc(D*MaxLen*sizeof(int));
	Orders = (int*)malloc(D*MaxLen*sizeof(int));
	Lengths = (int*)malloc(D*sizeof(int));
	Labels = (int*)malloc(D*sizeof(int));
	WordCounts = (int*)malloc(T*V*sizeof(int));
	SumWordCounts = (int*)malloc(T*sizeof(int));
	DocCounts = (int*)malloc(K*T*sizeof(int));
	Topics = (int*)malloc(D*sizeof(int));
	Prob = (double*)malloc(T*sizeof(double));
	MeanWordCounts = (int*)malloc(V*sizeof(int));
	MeanDocCounts = (int*)malloc(K*sizeof(int));
	
	srand((unsigned)time(NULL));
	Initialize();
	Temp = (int*)malloc(D*sizeof(int));
	tTemp = (int*)malloc(T*sizeof(int));
	
	total = 0;
	for(i = 0;i < D;i++)
	{
		*(Temp+i) = 0;
		total += *(Lengths+i);
	}
	for(t = 0;t < iter_num;t++)
	{
		printf("%d ",t+1);
		SampleForDocs();
		dis = 0;
		for(i = 0;i < D;i++)
		{
			if(*(Temp+i) != *(Topics+i))
			{
				dis++;
				*(Temp+i) = *(Topics+i);
			}
		}
		printf("%f ",dis/(float)D);
		printf("%d ",TopicNums(tTemp));
		printf("%f\n",Perplexity());
	}
	free(Temp);
	free(tTemp);
	
	Temp = (int*)malloc(D*T*sizeof(int));
	TopicVote(Temp,duration);
	InitializeWithTopics();
	free(Temp);
	
	Temp = (int*)malloc(T*sizeof(int));
	printf("%d\n",TopicNums(Temp));
	free(Temp);
	
	printf("%f\n",Perplexity());
	
	WriteFile();
	
	free(Words);
	free(Orders);
	free(Lengths);
	free(Labels);
	free(WordCounts);
	free(SumWordCounts);
	free(DocCounts);
	free(Topics);
	free(Prob);
	free(MeanWordCounts);
	free(MeanDocCounts);
	return;
}
