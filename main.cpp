#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <algorithm>

#define maxx 90000
#define runtimes 1000
#define initnum 6
#define thres1 100
#define thres2 50

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
	double result=(double) rand() / (RAND_MAX + 1.0 );
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
void random_choice_initnode(){
	for(int i=0,j;i<initnum;i++){	//choose initnode
		do{
			j=rand()%nummax;
		}while(user[j].active==1 || exist[j]==0);
		//if this user is selected or this user is not existed
		//choose new user again
		initnode[i]=j;
		//user[j].active=1;
	}
}

void choice_initnode(int by_choice[],int num){
	for(int i=0;i<num;i++)
		initnode[i]=by_choice[i];
}

void friend_choice_initnode(){
	int j;
	do{
		j=rand()%nummax;
	}while(exist[j]==0 || user[j].friends.size()<3);
	initnode[0]=j;
	for(int i=1;i<initnum;i++){
		do{
			j=rand()%user[initnode[i-1]].friends.size();
			j=user[initnode[i-1]].friends[j].id;
		}while(user[i].active==1 || user[j].friends.size()<3);
		initnode[i]=j;
	}
}
int check[maxx];	//check whether particular round has put the seed

int main(int argc,char* argv[]){
	int a,b;
	double c;
	queue<int>infl;		//to do bfs
	int influence_times[maxx];
	int result_num[100]={0},sum=0;
	FILE *read_edge,*out,*ccc;

	nummax=0;	//nummax to record the max id
	read_edge=fopen(argv[1],"r");
	out=fopen("round_put.txt","w");
	ccc=fopen("check.txt","w");
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
	//5381	2.014125	704.095000
	//23502	2.010466	814.617000
	//15747	2.027851	841.447000
	//8031	2.248835	1794.408000
	//23151	2.235933	1627.406000
	//42482	2.260170	2304.079000

	//int by_choice[]={5381, 23502, 15747, 23151, 42482, 8031};
	int by_choice[]={8031, 23151, 42482, 15747, 23502, 5381};
	choice_initnode(by_choice,initnum);

	int initnode_be_effected[10]={0};		//count it is active node but it have been effected times
	bool put_activenode[10]={0};	//to put active node for the some round
	int putt,put_time[10]={0,3,10,15,19,24};	//the round should be put
									//putt record the  which round is next round 	
	//The follow is run the experence
	//it is round (runtimes) times to record the sum of each experence result
	//then average the result
	for(int t=0;t<runtimes;t++){
		for(int i=0;i<nummax;i++){	//initial each node
			user[i].active=0;
			user[i].round_time=-1;
		}
		memset(check,0,sizeof(check));
		////////////////////////////
		/*for(int i=0;i<initnum;i++){	//put all initial node to queue
			user[initnode[i]].active=1;
			user[initnode[i]].round_time=0;
			infl.push(initnode[i]);
		}*/
		/////////////////////////////
		putt=0;
		memset(put_activenode,0,sizeof(put_activenode));
		int x=0;
		int tmp,tmp_round=0;	//tmp_round record round_time of queue.front
		while(!infl.empty() || !is_init_put(initnode)){
			//for each round to put the act node once, and check the node
			//has been influenced. if yes record this, no put it into queue
			//////////////////////////////////////////////////////////////////////////////////
			if(put_time[putt]==tmp_round && putt<initnum && put_activenode[putt]==false){
				put_activenode[putt]=true;
				if(user[initnode[putt]].active==0){
					infl.push(initnode[putt]);
					user[initnode[putt]].active=1;
					user[initnode[putt]].round_time=tmp_round;
				}
				else if(putt<initnum){
					initnode_be_effected[putt]++;
				}
				putt++;
			}
			/////////////////////////////////////////////////////////////////////////////////
			if(!infl.empty()){
				tmp=infl.front();
				infl.pop();
				if(check[tmp] != 1){
					tmp_round=user[tmp].round_time;
					result_num[tmp_round]++;
				}
				//check[x++]=tmp;
			}
			else{
				tmp_round=put_time[putt];
				continue;
			}
			//printf("tmp_round=%d %d %d\n",tmp_round,putt,put_activenode[putt]);
			int friendnum=user[tmp].friends.size();
			for(int i=0,j;i<friendnum;i++){
				j=user[tmp].friends[i].id;
				if(!user[j].active && take_random()<user[tmp].friends[i].rand){
					infl.push(j);
					user[j].round_time=user[tmp].round_time+1;
					user[j].active=1;
				}
			}
		}//while(!infl.empty())
	}//for(runtimes)
	
	//print the result
	//printf("init node:\n");
	//for(int i=0;i<initnum;i++)
	//	printf("%4d\n",initnode[i]);
	int thres1_up=0, thres2_up=0;
	fprintf(out,"The initial node and its num of friends\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%4d\t%4d\t%.3lf\n",initnode[i],user[initnode[i]].friends.size()
				,(double)user[initnode[i]].expect);
	fprintf(out,"put round\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%3d\t",put_time[i]);
	fprintf(out,"\ninit node be influeced before active times\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%3d\t",initnode_be_effected[i]);
	fprintf(out, "\n");
	for(int i=0;result_num[i]!=0 || i<put_time[initnum];i++){		//caulate the average of each round
		sum+=result_num[i];
		int tmp=result_num[i]/(double)runtimes;
		if(tmp>thres1)	thres1_up++;
		if(tmp>thres2)	thres2_up++;
	}
	fprintf(out,"%4d up:\t%3d\n%4d up:\t%3d\n",thres1,thres1_up,thres2,thres2_up);
	fprintf(out,"the average num: \n%.3lf\n",sum/(double)runtimes);
	fprintf(out,"average round\n");
	for(int i=0;result_num[i]!=0;i++)
		fprintf(out,"%.3lf\n",result_num[i]/(double)runtimes);
	
	fclose(read_edge);
	fclose(out);
	return 0;
}
