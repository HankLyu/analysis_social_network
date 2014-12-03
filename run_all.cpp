#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <algorithm>
#include <string>

#define maxx 90000
#define runtimes 1000
#define initnum 1
#define random 10000	//for random from 0.0001 to 1

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
	bool exist;
	int effect_times;
};
bool social_cmp(social a, social b){
	return a.expect>b.expect;
}

struct combination{
	char people[50];
	int num;
	double expect;
};
bool comb_cmp(combination a, combination b){
	return a.expect > b.expect;
}

social user[maxx];
//int exist[maxx];
int initnode[initnum];
int nummax;		//nummax to record the max id
vector<combination> comb;

double take_random(){
	//srand(time(NULL));
	double result=rand() % random;
	result = result / (double)random;
	return result;
}

bool is_init_put(int initnode[]){
	for(int i=0;i<initnum;i++)
		if(user[initnode[i]].active==0)
			return false;
	return true;
}

int main(int argc,char* argv[]){
	int a,b;
	double c;
	queue<int>infl;		//to do bfs
	int influence_times[maxx];
	int result_num[100]={0},sum=0;
	FILE *read_edge,*out;

	nummax=0;	//nummax to record the max id
	read_edge=fopen(argv[1],"r");
	out=fopen("run_all.txt","w");
	memset(user,0,sizeof(user));
	friending inputfriend;

	while(fscanf(read_edge,"%d %d %lf",&a,&b,&c) != EOF){	//read the adge
		user[a].id=a;
		inputfriend.id=b;
		inputfriend.rand=c;
		user[a].friends.push_back(inputfriend);
		user[a].expect+=c;
		nummax=max(nummax,a);
		user[a].exist=1;
	}
	srand(time(NULL));
	nummax++;	//it is convience for for_loop

	int tmp_num=0;

	bool put_activenode[10]={0};	//to put active node for the some round
	int put_time[10]={0,3,6,9,12,15,18,21,24,27},putt;	//the round should be put
									//putt record the  which round is next round 	
	//The follow is run the experence
	//it is round (runtimes) times to record the sum of each experence result
	//then average the result

	combination comb_tmp;
	for(int x=0;x<nummax;x++){
		if(!user[x].exist)	continue;
		initnode[0]=x;
		memset(result_num,0,sizeof(result_num));
	for(int t=0;t<runtimes;t++){
		for(int i=0;i<nummax;i++){	//initial each node
			user[i].active=0;
			user[i].round_time=-1;
		}
		////////////////////////////
		/*for(int i=0;i<initnum;i++){	//put all initial node to queue
			user[initnode[i]].active=1;
			user[initnode[i]].round_time=0;
			infl.push(initnode[i]);
		}*/
		/////////////////////////////
		putt=0;
		memset(put_activenode,0,sizeof(put_activenode));
		//int x=0;
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
				putt++;
			}
			/////////////////////////////////////////////////////////////////////////////////
			if(!infl.empty()){
				tmp=infl.front();
				infl.pop();
				tmp_round=user[tmp].round_time;
				result_num[tmp_round]++;
				//check[x++]=tmp;
			}
			else if(putt<initnum){
				tmp_round=put_time[putt];
				continue;
			}
			//user[tmp].effect_times++;
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
				tmp_num=0;
				for(int i=0;result_num[i]!=0;i++)
					tmp_num+=result_num[i];
				comb_tmp.num=tmp_num;
				comb_tmp.expect=user[initnode[0]].expect;
				sprintf(comb_tmp.people,"%d"
					,initnode[0],initnode[1],initnode[2],initnode[3]
					,initnode[4],initnode[5],initnode[6],initnode[7]
					,initnode[8],initnode[9]);
				comb.push_back(comb_tmp);
				printf("%s   %.3lf\n",comb_tmp.people
					,comb_tmp.num/(double)runtimes);
	}

	sort(comb.begin(),comb.end(),comb_cmp);
	for(int i=0;i<comb.size();i++)
		fprintf(out,"%s\t%lf\t%3lf\n",comb[i].people
			,comb[i].expect, comb[i].num/(double)runtimes);
	fclose(read_edge);
	fclose(out);
	return 0;
}
