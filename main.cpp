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

social user[maxx];
int exist[maxx];
int nummax;		//nummax to record the max id

double take_random();
bool is_all_init_put(int seed[]);
void random_choice_seed(int seed[]);
void choice_seed(int by_choice[],int num, int seed[]);
void run_result(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[]);


int main(int argc,char* argv[]){
	int a,b;
	double c;
	int seed[initnum];
	int influence_times[maxx];
	double influenced_num_round[100]={0},sum=0;
	FILE *read_edge,*out;

	nummax=0;	//nummax to record the max id
	read_edge=fopen(argv[1],"r");
	out=fopen("round_put.txt","w");
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

	//random_choice_seed();
	//friend_choice_seed();
	//9881, 30635, 8932

	//int by_choice[]={19777, 9982, 2813, 7052, 248, 5920};
	int by_choice[]={19777, 9982, 2813, 5920, 248, 7052};
	choice_seed(seed, initnum, seed);

	int seed_be_effected[10]={0};		//count it is active node but it have been effected times
	int put_time[10]={0,11,17,24,29,30};	//the round should be put
	
	run_result(by_choice, put_time, influenced_num_round, seed_be_effected);
	//print the result
	//printf("init node:\n");
	//for(int i=0;i<initnum;i++)
	//	printf("%4d\n",seed[i]);
	int thres1_up=0, thres2_up=0;
	fprintf(out,"The initial node and its num of friends\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%4d\t%4d\t%.3lf\n",seed[i],user[seed[i]].friends.size()
				,(double)user[seed[i]].expect);
	fprintf(out,"put round\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%3d\t",put_time[i]);
	fprintf(out,"\ninit node be influeced before active times\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%3d\t",seed_be_effected[i]);
	fprintf(out, "\n");
	for(int i=0;influenced_num_round[i]!=0 || i<put_time[initnum];i++){		//caulate the average of each round
		sum+=influenced_num_round[i];
		if(influenced_num_round[i]>thres1)	thres1_up++;
		if(influenced_num_round[i]>thres2)	thres2_up++;
	}
	fprintf(out,"%4d up:\t%3d\n%4d up:\t%3d\n",thres1,thres1_up,thres2,thres2_up);
	fprintf(out,"the average num: \n%.3lf\n",sum);
	fprintf(out,"average round\n");
	for(int i=0;influenced_num_round[i]!=0;i++)
		fprintf(out,"%.3lf\n",influenced_num_round[i]);
	
	fclose(read_edge);
	fclose(out);
	return 0;
}


double take_random(){
	double result=(double) rand() / (RAND_MAX + 1.0 );
	return result;
}
bool is_all_init_put(int seed[]){
	for(int i=0;i<initnum;i++)
		if(user[seed[i]].active==0)
			return false;
	return true;
}
void random_choice_seed(int seed[]){
	for(int i=0,j;i<initnum;i++){	//choose seed
		do{
			j=rand()%nummax;
		}while(user[j].active==1 || exist[j]==0);
		//if this user is selected or this user is not existed
		//choose new user again
		seed[i]=j;
		//user[j].active=1;
	}
}

void choice_seed(int by_choice[], int num, int seed[]){
	for(int i=0;i<num;i++)
		seed[i]=by_choice[i];
}

void run_result(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[]){
	bool put_activenode[10]={0};	//to put active node for the some round
	int next_seed;						//next_seed record the  which round is next round 	
	int times_result_num[100]={0};
	queue<int>infl;		//to do bfs
	//The follow is run the experence
	//it is round (runtimes) times to record the sum of each experence result
	//then average the result
	for(int t=0;t<runtimes;t++){
		while(!infl.empty())	infl.pop();
		for(int i=0;i<nummax;i++){	//initial each node
			user[i].active=0;
			user[i].round_time=-1;
		}
		////////////////////////////
		/*for(int i=0;i<initnum;i++){	//put all initial node to queue
			user[seed[i]].active=1;
			user[seed[i]].round_time=0;
			infl.push(seed[i]);
		}*/
		/////////////////////////////
		next_seed=0;
		memset(put_activenode,0,sizeof(put_activenode));
		int x=0;
		int tmp,tmp_round=0;	//tmp_round record round_time of queue.front
		while(!infl.empty() || !is_all_init_put(seed)){
			//for each round to put the act node once, and check the node
			//has been influenced. if yes record this, no put it into queue
			//////////////////////////////////////////////////////////////////////////////////
			if(put_time[next_seed]==tmp_round && next_seed<initnum && put_activenode[next_seed]==false){
				put_activenode[next_seed]=true;
				if(user[seed[next_seed]].active==0){
					infl.push(seed[next_seed]);
					user[seed[next_seed]].active=1;
					user[seed[next_seed]].round_time=tmp_round;
				}
				else if(next_seed < initnum){
					seed_be_effected[next_seed]++;
				}
				next_seed++;
			}
			/////////////////////////////////////////////////////////////////////////////////
			if(!infl.empty()){
				tmp=infl.front();
				infl.pop();
				tmp_round=user[tmp].round_time;
				times_result_num[tmp_round]++;
			}
			else{
				tmp_round=put_time[next_seed];
				continue;
			}
			//printf("tmp_round=%d %d %d\n",tmp_round,next_seed,put_activenode[next_seed]);
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
	for(int i=0;times_result_num[i]!=0 || i<put_time[initnum];i++){		//caulate the average of each round
		influenced_num_round[i] = times_result_num[i]/(double)runtimes;
	}
}