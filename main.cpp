#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <queue>

#define maxx 90000
#define runtimes 1000
#define initnum 3
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

void choice_initnode(int a, int b, int c){
	initnode[0]=a;
	initnode[1]=b;
	initnode[2]=c;
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
int main(int argc,char* argv[]){
	int a,b;
	double c;
	queue<int>infl;		//to do bfs
	int influence_times[maxx];
	int result_num[100]={0},sum=0;
	FILE *read_edge,*out;

	nummax=0;	//nummax to record the max id
	read_edge=fopen(argv[1],"r");
	out=fopen("round_put.txt","w");
	memset(user,0,sizeof(user));
	friending inputfriend;

	while(fscanf(read_edge,"%d %d %lf",&a,&b,&c) != EOF){	//read the adge
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
	choice_initnode(7363,6281,3971);

	int initnode_be_effected[10]={0};		//count it is active node but it have been effected times
	bool put_activenode[10]={0};	//to put active node for the some round
	int put_time[10]={0,4,8},putt;	//the round should be put
									//putt record the  which round is next round 	
	//The follow is run the experence
	//it is round (runtimes) times to record the sum of each experence result
	//then average the result
	for(int t=0;t<runtimes;t++){
		for(int i=0;i<nummax;i++){	//initial each node
			user[i].active=0;
			user[i].round_time=-1;
		}
		/*for(int i=0;i<initnum;i++){	//put all initial node to queue
			user[initnode[i]].active=1;
			user[initnode[i]].round_time=0;
			infl.push(initnode[i]);
		}*/
		putt=0;
		memset(put_activenode,0,sizeof(put_activenode));
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
				else{
					initnode_be_effected[putt]++;
				}
				putt++;
			}
			/////////////////////////////////////////////////////////////////////////////////
			if(!infl.empty()){
				tmp=infl.front();
				infl.pop();
				tmp_round=user[tmp].round_time;
				result_num[tmp_round]++;
			}
			else{
				tmp_round=put_time[putt];
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
	fprintf(out,"The initial node and its num of friends\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%4d %4d     %.2lf\n",initnode[i],user[initnode[i]].friends.size()
				,(double)user[initnode[i]].expect);
	fprintf(out,"init node be influeced before active times\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%3d     ",initnode_be_effected[i]);
	fprintf(out,"\naverage round\n");
	for(int i=0;result_num[i]!=0 || i<put_time[2];i++){		//caulate the average of each round
		sum+=result_num[i];
		//printf("%d\n",result_num[i]);
		fprintf(out,"%.3lf\n",result_num[i]/(double)runtimes);
	}
	fprintf(out,"the average num: \n%.3lf\n",sum/(double)runtimes);
	return 0;
}
