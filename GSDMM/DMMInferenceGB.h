#if !defined(DMMINFERENCEGB_H)
#define DMMINFERENCEGB_H

void TopicVote(int * Temp,int duration);
int TopicNums(int * Temp);
double Perplexity();
void DMMInferenceGB(double param_alpha,double param_beta,int topic_num,int iter_num,int duration);

#endif
