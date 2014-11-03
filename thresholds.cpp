#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <algorithm>

#define maxx 90000
#define runtimes 1
#define initnum 10
#define random 10000	//for random from 0.0001 to 1
#define thresholds 7
#define delaytime 2	//dont let initnode be put continusly

using namespace std;

struct friending{
	int id;
	double rand;
};

struct social{
	int id;
	bool active;	//if 1 this is be infected, if 0, otherwise
	int round_time;
	double expect;
	vector<friending> friends;
};

double take_random(){
	//srand(time(NULL));
	double result=rand() % random;
	result = result / (double)random;
	return result;
}

social user[maxx];
int exist[maxx];
int initnode[initnum];
int nummax;		//nummax to record the max id

bool is_init_put(int initnode[]){
	for(int i=0;i<initnum;i++)
		if(user[initnode[i]].active==0)
			return false;
	return true;
}

void choice_initnode(int by_choice[],int num){
	for(int i=0;i<num;i++)
		initnode[i]=by_choice[i];
}

int main(int argc,char* argv[]){
	int a,b;
	double c;
	queue<int>infl;		//to do bfs
	int influence_times[maxx];
	int result_num[100]={0},sum=0;
	int putround[100]={0};
	FILE *read_edge,*out;

	nummax=0;	//nummax to record the max id
	read_edge=fopen(argv[1],"r");
	out=fopen("thresholds_put.txt","w");
	memset(user,0,sizeof(user));
	friending inputfriend;

	while(fscanf(read_edge,"%d %d %lf",&a,&b,&c) != EOF){	//read the adge
		user[a].id=a;
		inputfriend.id=b;
		inputfriend.rand=c;
		user[a].friends.push_back(inputfriend);
		user[a].expect+=c;
		nummax=max(nummax,a);
		exist[a]++;
	}
	srand(time(NULL));
	nummax++;	//it is convience for for_loop

	//random_choice_initnode();
	//friend_choice_initnode();
	int by_choice[]={40, 250, 159, 108, 116, 651, 191, 44, 6895, 36};
	choice_initnode(by_choice,initnum);

	bool put_activenode[10]={0};	//to put active node for the some round
	int thres_put;	//the round should be put

	//The follow is run the experence
	//it is round (runtimes) times to record the sum of each experence result
	//then average the result
	for(int t=0;t<runtimes;t++){
		for(int i=0;i<nummax;i++){	//initial each node
			user[i].active=0;
			user[i].round_time=-1;
		}
		thres_put=1;
		user[initnode[0]].active=1;
		user[initnode[0]].round_time=0;
		infl.push(initnode[0]);
		memset(put_activenode,0,sizeof(put_activenode));
		put_activenode[0]=true;
		int delayput=delaytime;
		int tmp_result[100]={0};	//record each runtime for thresholds use
		int tmp,tmp_round=0;	//tmp_round record round_time of queue.front
		while(!infl.empty() || !is_init_put(initnode)){
			//for each round to put the act node once, and check the node
			//has been influenced. if yes record this, no put it into queue
			//////////////////////////////////////////////////////////////////////////////////
			if((tmp_round>1 && tmp_result[tmp_round-1]<thresholds && 
				thres_put<(initnum-1) && put_activenode[tmp_round]==false && tmp_round-delayput>=2)
				|| (infl.empty() && thresholds<(initnum-1))) {
				while(user[initnode[thres_put]].active != 0){
					thres_put++;
				}
				delayput=tmp_round;
				put_activenode[tmp_round]=true;
				infl.push(initnode[thres_put]);
				user[initnode[thres_put]].active=1;
				user[initnode[thres_put]].round_time=tmp_round;
				putround[thres_put]=tmp_round;
			}
			/////////////////////////////////////////////////////////////////////////////////
			if(!infl.empty()){
				tmp=infl.front();
				infl.pop();
				tmp_round=user[tmp].round_time;
				result_num[tmp_round]++;
				tmp_result[tmp_round]++;
				int friendnum=user[tmp].friends.size();
				for(int i=0,j;i<friendnum;i++){
					j=user[tmp].friends[i].id;
					if(!user[j].active && take_random()<user[tmp].friends[i].rand){
						infl.push(j);
						user[j].round_time=user[tmp].round_time+1;
						user[j].active=1;
					}
				}
			}
			//printf("tmp_round=%d %d %d\n",tmp_round,putt,put_activenode[putt]);
		}//while(!infl.empty())
		printf("\n");
		int check=0;
		for(int i=0;i<initnum;i++){
			if(user[initnode[i]].active==1)	check++;
		}
		printf("%d\n", check);
	}//for(runtimes)
	
	//print the result
	//printf("init node:\n");
	//for(int i=0;i<initnum;i++)
	//	printf("%4d\n",initnode[i]);
	fprintf(out,"The initial node and its num of friends and this round be put\n");
	fprintf(out,"id     friend num  expect      round\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%4d      %4d     %4.3lf     %2d\n",initnode[i], user[initnode[i]].friends.size()
				, user[initnode[i]].expect, putround[i]);
	fprintf(out,"init node be influeced before active times\n");
	for(int i=0;result_num[i]!=0;i++){		//caulate the average of each round
		sum+=result_num[i];
		//printf("%d\n",result_num[i]);
		fprintf(out,"%.3lf\n",result_num[i]/(double)runtimes);
	}
	fprintf(out,"the average num: \n%.3lf\n",sum/(double)runtimes);
	
	fclose(read_edge);
	fclose(out);
	return 0;
}
