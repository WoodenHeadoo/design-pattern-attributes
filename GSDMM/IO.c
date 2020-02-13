#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "IO.h"
#include "Iterate.h"
#include "globals.h"

extern int K,T,D,V,MaxLen;
extern int * Words,* Orders;
extern int * Lengths;
extern int * Labels;
extern int * WordCounts,* SumWordCounts;
extern int * DocCounts;
extern int * Topics;
extern int MeanSumWordCounts;
extern int * MeanWordCounts,* MeanDocCounts;

void GetDataScale()
{
	int i,j,k,cur,nums;
	FILE * fp;
	fp = fopen("tokens.txt","r");
	cur = 0,D = 0,V = 0,MaxLen = 0;
	while(fscanf(fp,"%d",&i) == 1)
	{
		if(i > cur)
		{
			cur = i;
			nums = 0;
			if(i > D)
			{
				D = i;
			}
		}
		fscanf(fp,"%d",&j);
		if(j > V)
		{
			V = j;
		}
	    fscanf(fp,"%d",&k);
	    nums += k;
	    if(nums > MaxLen)
	    {
	    	MaxLen = nums;
		}
	}
	fclose(fp);
	
	K = 0;
	fp = fopen("labels.txt","r");
	while(fscanf(fp,"%d",&i) == 1)
	{
		if(i > K)
		{
			K = i;
		}
	}
	return;
}

void InitializeWithTopics()
{
	int i,j,k,v,label,topic;
	for(i = 0;i < T;i++)
	{
		*(SumWordCounts+i) = 0;
		for(j = 0;j < V;j++)
		{
			*(WordCounts+i*V+j) = 0;
		}
		for(k = 0;k < K;k++)
		{
			*(DocCounts+k*T+i) = 0;
		}
	}
	for(i = 0;i < D;i++)
	{
		topic = *(Topics+i);
		label = *(Labels+i);
		*(DocCounts+label*T+topic) += 1;
		*(SumWordCounts+topic) += *(Lengths+i);
		for(k = 0;k < *(Lengths+i);k++)
		{
			v = *(Words+i*MaxLen+k);
			*(WordCounts+topic*V+v) += 1;
		}
	}
	return;
}

void Initialize()
{
	int row,col,num,cur,pos,i;
	FILE * fp;
	fp = fopen("tokens.txt","r");
	
	MeanSumWordCounts = 0;
	for(i = 0;i < V;i++)
	{
		*(MeanWordCounts+i) = 0;
	}
	cur = 0;
	while(fscanf(fp,"%d",&row) == 1)
	{
		if(row > cur)
		{
			cur = row;
			*(Lengths+cur-1) = 0;
			pos = 0;
		}
		fscanf(fp,"%d",&col);
	    fscanf(fp,"%d",&num);
		for(i = 0;i < num;i++)
		{
			*(Words+(cur-1)*MaxLen+pos) = col-1;
			*(Orders+(cur-1)*MaxLen+pos) = i;
			pos++;
		}
		*(Lengths+cur-1) += num;
		MeanSumWordCounts += num;
		*(MeanWordCounts+col-1) += num;
	}
	fclose(fp);
	MeanSumWordCounts /= T;
	for(i = 0;i < V;i++)
	{
		*(MeanWordCounts+i) /= T;
	}
	
	for(i = 0;i < K;i++)
	{
		*(MeanDocCounts+i) = 0;
	}
	cur = 0;
	fp = fopen("labels.txt","r");
	while(fscanf(fp,"%d",&i) == 1)
	{
		*(Labels+cur) = i-1;
		*(MeanDocCounts+i-1) += 1;
		cur++;
	}
	for(i = 0;i < K;i++)
	{
		*(MeanDocCounts+i) /= T;
	}
	
	/*for(i = 0;i < D;i++)
	{
		*(Topics+i) = (int)(rand()/(RAND_MAX+0.1)*T);
	}
	InitializeWithTopics();*/
	InitialSampling();
	
	return;
}

void WriteFile()
{
	int i,j,k,topic,v;
	double likeli,p;
	FILE * fp;
	
	fp = fopen("Topics.txt","w");
	for(i = 0;i < D;i++)
	{
		fprintf(fp,"%d\n",*(Topics+i)+1);
	}
	fclose(fp);
	fp = fopen("LabelTopics.txt","w");
	for(k = 0;k < K;k++)
	{
		for(i = 0;i < T;i++)
		{
			j = *(DocCounts+k*T+i);
			if(j > 0)
			{
				fprintf(fp,"%d %d %d\n",k+1,i+1,j);
			}
		}
	}
	fclose(fp);
	fp = fopen("TopicWords.txt","w");
	for(j = 0;j < T;j++)
	{
		for(k = 0;k < V;k++)
		{
			i = *(WordCounts+j*V+k);
			if(i > 0)
			{
				fprintf(fp,"%d %d %d\n",j+1,k+1,i);
			}
		}
	}
	fclose(fp);
	fp = fopen("MeanDocLikelihood.txt","w");
	for(i = 0;i < D;i++)
	{
		likeli = 0.0;
		topic = *(Topics+i);
		for(k = 0;k < *(Lengths+i);k++)
		{
			v = *(Words+i*MaxLen+k);
			p = (*(WordCounts+topic*V+v) + beta)/(*(SumWordCounts+topic) + V*beta);
			likeli += log(p); 
		}
		likeli /= *(Lengths+i);
		fprintf(fp,"%f\n",likeli);
	}
	fclose(fp);
}
