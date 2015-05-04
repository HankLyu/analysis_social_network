#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <algorithm>

#define maxx 200000
#define runtimes 1000
#define initnum 20
#define thres1 100
#define thres2 50
#define err 3

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
bool is_all_init_put(int seed[], int seednum);
void random_choice_seed(int seed[]);
void choice_seed(int by_choice[],int num, int seed[]);
void run_result(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum);
void run_puttime(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum, int startnum);
void greedy(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum);

int main(int argc,char* argv[]){
	int a,b;
	double c;
	int seed[initnum+5];
	int influence_times[maxx];
	double influenced_num_round[initnum*20]={0},sum=0;
	FILE *read_edge,*out;
	time_t start_time, finish_time;

	nummax=0;	//nummax to record the max id
	read_edge=fopen(argv[1],"r");
	out=fopen("random_put.txt","w");
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

	//random_choice_seed(seed);
	//friend_choice_seed();
	//9881, 30635, 8932

	int by_choice[]={35839, 42945, 36248, 19703, 20277, 688, 14580, 79318, 13153, 3776, 58658, 43689, 23784, 6340, 47163, 42988, 45865, 9330, 83423, 54685, 35923, 37251, 23577, 51697, 101694, 30497, 45678, 47217, 26589, 32049};
	//int by_choice[]={14613, 25069, 42384, 9620, 5657, 7037};
	choice_seed(by_choice, initnum, seed);
	for(int i=0;i<initnum;i++)
		printf("%d ",seed[i]);
	printf("\n");

	int seed_be_effected[initnum+5]={0};		//count it is active node but it have been effected times
	int put_time[initnum+5]={0,2,4,5,13,23};	//the round should be put
	
	start_time = time(NULL);
	//greedy(seed, put_time, influenced_num_round, seed_be_effected, initnum);
	run_puttime(seed, put_time, influenced_num_round, seed_be_effected, initnum, 0);
	memset(seed_be_effected,0,sizeof(seed_be_effected));
	run_result(seed, put_time, influenced_num_round, seed_be_effected, initnum);
	finish_time = time(NULL);
	std::cout<<"start time "<<start_time<<endl << "end time "<< finish_time<<endl;
	std::cout<< "the random time is "<<(finish_time - start_time)<<endl;
	fprintf(out,"The random run time is %d\n", (finish_time - start_time));
	memset(seed_be_effected,0,sizeof(seed_be_effected));
	//print the result
	//printf("init node:\n");
	//for(int i=0;i<initnum;i++)
	//	printf("%4d\n",seed[i]);
	int thres1_up=0, thres2_up=0;
	fprintf(out,"Greedy:\nThe initial node and its num of friends\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%4d\t%4d\t%.3lf\n",seed[i],user[seed[i]].friends.size()
				,(double)user[seed[i]].expect);
	fprintf(out,"the order of seeds\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%4d, ", seed[i]);
	fprintf(out,"\nput round\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%3d, ",put_time[i]);
	fprintf(out, "\n");
	bool upto_thres=false;
	for(int k=0;influenced_num_round[k]> 0.0; k++){		//caulate the average of each round
		if(influenced_num_round[k] > thres1)	upto_thres=true;
		if(upto_thres){
			//printf("tmp_num_round %d\n", tmp_num_round);
			if(influenced_num_round[k]>thres1)	thres1_up++;
			else break;
		}
	}
	for(int i=0;influenced_num_round[i]> 0.0;i++){		//caulate the average of each round
		sum+=influenced_num_round[i];
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
bool is_all_init_put(int seed[], int seednum){
	for(int i=0;i<seednum;i++)
		if(user[seed[i]].active==0)
			return false;
	return true;
}
void random_choice_seed(int seed[]){
	double sum, tmp_influence[initnum*20];
	int tmp_seed[5], tmp_puttime[5], tmp_effect[5];
	for(int i=0,j;i<initnum;i++){	//choose seed
		while(1){
			j= rand()%nummax;
			if(user[j].active==1 || exist[j]==0)	continue;
			tmp_seed[0]=j;
			run_result(tmp_seed, tmp_puttime, tmp_influence, tmp_effect, 1);
			sum=0;
			for(int k=0; tmp_influence[k]>0.0 && k<100; k++){
				//printf("%lf\n", tmp_influence[k]);
				sum += tmp_influence[k];
			}
			//printf("sum:%lf\n", sum);
			if(sum > thres1*7)
				break;
		}
		printf("seed: %d id: %d effected num: %lf\n", i, j, sum);
		//if this user is selected or this user is not existed
		//choose new user again
		seed[i]=j;
		//user[j].active=1;
	}
}

void choice_seed(int by_choice[], int num, int seed[]){
	bool chose[initnum]={0};
	int randnum;
	for(int i=0;i<num;i++){
		do{	//this comment is to do random order for seed
			randnum = rand()%num;
		}
		while(chose[randnum] == true);
		chose[randnum]=true;
		
		//printf("%d\n", randnum);
		//randnum=i;
		seed[randnum]=by_choice[i];
	}
}

void run_result(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum){
	bool put_activenode[initnum+5]={0};	//to put active node for the some round
	int next_seed;						//next_seed record the  which round is next round 	
	int times_result_num[initnum*20]={0};
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
		/*for(int i=0;i<seednum;i++){	//put all initial node to queue
			user[seed[i]].active=1;
			user[seed[i]].round_time=0;
			infl.push(seed[i]);
		}*/
		/////////////////////////////
		next_seed=0;
		memset(put_activenode,0,sizeof(put_activenode));
		int x=0;
		int tmp,tmp_round=0;	//tmp_round record round_time of queue.front
		while(!infl.empty() || !is_all_init_put(seed, seednum)){
			//for each round to put the act node once, and check the node
			//has been influenced. if yes record this, no put it into queue
			//////////////////////////////////////////////////////////////////////////////////
			if(put_time[next_seed]==tmp_round && next_seed<seednum && put_activenode[next_seed]==false){
				put_activenode[next_seed]=true;
				if(user[seed[next_seed]].active==0){
					infl.push(seed[next_seed]);
					user[seed[next_seed]].active=1;
					user[seed[next_seed]].round_time=tmp_round;
				}
				else if(next_seed < seednum){
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
	for(int i=0; i<initnum*20; i++){		//caulate the average of each round
		influenced_num_round[i] = times_result_num[i]/(double)runtimes;
		//printf("influenced_num_round %d %lf\n", i, influenced_num_round[i]);
	}
}

void run_puttime(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum, int startnum){
	int record_num=0;	//to make sure eac
	for(int i=startnum; i<seednum; i++){
		if(i == 0){
			put_time[0]=0;
			printf("choose first seed: %d\n", seed[0]);
			run_result(seed, put_time, influenced_num_round, seed_be_effected, i+1);
			continue;
		}
		int max_num_round=0, best_put_time=put_time[i-1], max_thres=0;
		int tmp_num_round;
		printf("i=%d\n",i);
		for(int j=put_time[i-1]; j<put_time[i-1]+15;j++){
			put_time[i]=j;
			run_result(seed, put_time, influenced_num_round, seed_be_effected, i+1);
			tmp_num_round=0;
			bool upto_thres=false;
			for(int k=0;influenced_num_round[k]> 0.0; k++){		//caulate the average of each round
				if(influenced_num_round[k] > thres1)	upto_thres=true;
				if(upto_thres){
					//printf("tmp_num_round %d\n", tmp_num_round);
					if(influenced_num_round[k]>thres1 + err)	tmp_num_round++;
					else break;
				}
			}
			//printf("%d %d %d\n", i, j, tmp_num_round);
			if(max_num_round < tmp_num_round){
				max_num_round = tmp_num_round;
				best_put_time = j;
			}
		}//for(int j=put_time[i]+1; j<put_time[i]+15;j++)
		if(max_num_round < record_num + 2)
			best_put_time = put_time[i-1];
		else	
			record_num = max_num_round;
		put_time[i]=best_put_time;
		printf("this seed %d: best_put_time is %d, round: %d\n", seed[i], put_time[i], max_num_round);
	}//for(int i=1; i<seednum; i++)
}
