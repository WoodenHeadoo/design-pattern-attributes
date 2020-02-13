#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "Iterate.h"
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
extern int MeanSumWordCounts;
extern int * MeanWordCounts,* MeanDocCounts;
//int flag;

int SampleWithProb()
{
	double ratio,sum;
	int i;
	sum = 0.0;
	for(i = 0;i < T;i++)
	{
		sum += *(Prob+i);
	}
	ratio = (rand()/(RAND_MAX+0.1))*sum;
	sum = 0.0;
	for(i = 0;i < T;i++)
	{
		sum += *(Prob+i);
		if(sum > ratio)
		{
			return i;
		}
	}
	printf("Error: %lf\n",sum);
	return T-1;
}

void InitialSampling()
{
	int i,j,k,v,label,topic;
	double temp,tmax;
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
		label = *(Labels+i);
		for(j = 0;j < T;j++)
		{
			temp = log(*(DocCounts+label*T+j) + alpha);
			for(k = 0;k < *(Lengths+i);k++)
			{
				v = *(Words+i*MaxLen+k);
				temp += log((*(WordCounts+j*V+v) + *(Orders+i*MaxLen+k) +beta)/(*(SumWordCounts+j) + k + V*beta));
			}
			*(Prob+j) = temp;
		}
		tmax = *Prob;
		for(j = 1;j < T;j++)
		{
			if(*(Prob+j) > tmax)
			{
				tmax = *(Prob+j);
			}
		}
		for(j = 0;j < T;j++)
		{
			*(Prob+j) = exp(*(Prob+j)-tmax);
		}
		topic = SampleWithProb();
		*(Topics+i) = topic;
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

void SampleForDocs()
{
	int i,j,k,v,label,topic;
	double temp,tmax;
	for(i = 0;i < D;i++)
	{
		label = *(Labels+i);
		topic = *(Topics+i);
		*(DocCounts+label*T+topic) -= 1;
		*(SumWordCounts+topic) -= *(Lengths+i);
		for(k = 0;k < *(Lengths+i);k++)
		{
			v = *(Words+i*MaxLen+k);
			*(WordCounts+topic*V+v) -= 1;
		}
		for(j = 0;j < T;j++)
		{
			//temp = (*(DocCounts+label*T+j) + alpha)/(*(MeanDocCounts+label) + alpha);
			temp = log(*(DocCounts+label*T+j) + alpha);
			for(k = 0;k < *(Lengths+i);k++)
			{
				v = *(Words+i*MaxLen+k);
				//temp *= (*(WordCounts+j*V+v) + *(Orders+i*MaxLen+k) +beta)/(*(MeanWordCounts+v) + *(Orders+i*MaxLen+k) +beta)*(MeanSumWordCounts + k + V*beta)/(*(SumWordCounts+j) + k + V*beta);
				temp += log((*(WordCounts+j*V+v) + *(Orders+i*MaxLen+k) +beta)/(*(SumWordCounts+j) + k + V*beta));
			}
			*(Prob+j) = temp;
		}
		tmax = *Prob;
		for(j = 1;j < T;j++)
		{
			if(*(Prob+j) > tmax)
			{
				tmax = *(Prob+j);
			}
		}
		for(j = 0;j < T;j++)
		{
			*(Prob+j) = exp(*(Prob+j)-tmax);
		}
		//flag = i;
		topic = SampleWithProb();
		*(Topics+i) = topic;
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
