#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <algorithm>
#include <stdint.h>

#define maxx 200000
#define runtimes 1000
#define initnum 30
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
	out=fopen("break_put.txt","w");
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

	//int by_choice[]={31626, 12501, 1621, 35460, 29204, 4841};
	int by_choice[]={35839, 42945, 36248, 19703, 20277, 688, 14580, 79318, 13153, 3776, 58658, 43689, 23784, 6340, 47163, 42988, 45865, 9330, 83423, 54685, 35923, 37251, 23577, 51697, 101694, 30497, 45678, 47217, 26589, 32049};
	choice_seed(by_choice, initnum, seed);

	int seed_be_effected[initnum+5]={0};		//count it is active node but it have been effected times
	int put_time[initnum+5]={0, 10, 10, 10, 10, 10};	//the round should be put
	
	start_time = time(NULL);
	int init[5];
	greedy(seed, put_time, influenced_num_round, seed_be_effected, initnum);
	memset(seed_be_effected,0,sizeof(seed_be_effected));
	run_result(seed, put_time, influenced_num_round, seed_be_effected, initnum);
	finish_time = time(NULL);
	std::cout<<"start time "<<start_time<<endl << "end time "<< finish_time<<endl;
	std::cout<<"The break run time is "<< (int)(finish_time - start_time) << endl;
	fprintf(out,"The break run time is %d\n", (finish_time - start_time));
	//run_puttime(seed, put_time, influenced_num_round, seed_be_effected, initnum, 0);
	//print the result
	//printf("init node:\n");
	//for(int i=0;i<initnum;i++)
	//	printf("%4d\n",seed[i]);
	int thres1_up=0, thres2_up=0;
	fprintf(out,"break:\nThe initial node and its num of friends\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%4d\t%4d\t%.3lf\n",seed[i],user[seed[i]].friends.size()
				,(double)user[seed[i]].expect);
	fprintf(out,"put round\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%3d\t",put_time[i]);
	fprintf(out,"\ninit node be influeced before active times\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%.3lf\t", (double)seed_be_effected[i]/(double)runtimes);
	fprintf(out, "\n");
	bool upto_thres=false;
	for(int k=0;influenced_num_round[k]> 1e-7; k++){		//caulate the average of each round
		if(influenced_num_round[k] > thres1)	upto_thres=true;
		if(upto_thres){
			//printf("tmp_num_round %d\n", tmp_num_round);
			if(influenced_num_round[k]>thres1)	thres1_up++;
			else break;
		}
	}
	for(int i=0;influenced_num_round[i]> 1e-7;i++){		//caulate the average of each round
		sum+=influenced_num_round[i];
		if(influenced_num_round[i]>thres2)	thres2_up++;
	}
	fprintf(out,"%4d up:\t%3d\n%4d up:\t%3d\n",thres1,thres1_up,thres2,thres2_up);
	fprintf(out,"the average num: \n%.3lf\n",sum);
	fprintf(out,"average round\n");
	for(int i=0;influenced_num_round[i]> 1e-7;i++)
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

void choice_seed(int by_choice[], int num, int seed[]){
	for(int i=0;i<num;i++)
		seed[i]=by_choice[i];
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
	}
}

void run_puttime(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum, int startnum){
	double best_influenced_num_round[initnum*20];
	for(int i=startnum; i<seednum; i++){
		if(i == 0){
			put_time[0]=0;
			printf("choose first seed: %d\n", seed[0]);
			run_result(seed, put_time, influenced_num_round, seed_be_effected, i+1);
			continue;
		}
		int max_num_round=0, best_put_time=put_time[i-1];
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
			if(max_num_round < tmp_num_round){		//check whether this put time have better progation
				max_num_round = tmp_num_round;
				best_put_time = j;
				memcpy(best_influenced_num_round, influenced_num_round, sizeof(best_influenced_num_round));
			}
			if(max_num_round > tmp_num_round){
				printf("doooo\n");
				break;
			}
		}//for(int j=put_time[i]+1; j<put_time[i]+15;j++)
		put_time[i]=best_put_time;
		printf("this seed %d: best_put_time is %d, round: %d\n", seed[i], put_time[i], max_num_round);
		memcpy(influenced_num_round, best_influenced_num_round, sizeof(best_influenced_num_round));	//return the best of the influence of this seed
	}//for(int i=1; i<seednum; i++)
}

void greedy(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum){
	int curnum=0, be_put[initnum]={0};
	int last_effect=0, seed_effect[initnum]={0};
	double best_influenced_num_round[initnum*20];
		//last_effect is to record how many rounds are over thres
		//seed_effect is to record how many rounds are over thres to check whether this seed have enough influence
	int best_put_order[initnum], begin_round=0, tmp_begin_round;
		//begin_round is to record the round of peak.
	int best_put_time;	//record the time of the current seed
	for(int i=0;i<curnum;i++){	//if the order of the part seeds is sure, change the "curnum" valure 
		be_put[i]=1;
		best_put_order[i]=seed[i];
	}
	while(curnum < seednum){
		int min_overthres=1e5;	//to find the  best seed having the min peak for this order to make sure the wave is not too high
		int peak;	//to find the peak of each seed
		int bestseed_thisround=-1;	//to record which seed is best in this round
		
		for(int i=0; i<seednum; i++){ //to 
			peak=0;
			if(be_put[i] == 1)	continue;

			best_put_order[curnum] = seed[i];
			run_puttime(best_put_order, put_time, influenced_num_round, seed_be_effected, curnum+1, curnum);
			tmp_begin_round=begin_round;
			//chose the peak
			for(int j=tmp_begin_round; j < tmp_begin_round+35; j++){	//check the influencu wave
				//printf("%lf\n",influenced_num_round[j]);
				if(peak < (int)influenced_num_round[j]){
					peak = (int)influenced_num_round[j];
				}
				peak=max(peak, (int)influenced_num_round[j]);
			}
			printf(" peak: %d\n", peak);
			bool upto_thres=false;
			for(int k=0;influenced_num_round[k]> 0.0; k++){		//caulate the average of each round
				if(influenced_num_round[k] > thres1)	upto_thres=true;
				if(upto_thres){
					if(influenced_num_round[k]>thres1)	seed_effect[i]++;
					else break;
				}
			}
			//printf("seed %d max over %d\n", seed[i], peak);
			if(peak < min_overthres && peak>thres1
				&& seed_effect[i] > last_effect){	//check whether the current seed is good
				min_overthres=peak;	//to sure the new seed just over the threshold a bit
				bestseed_thisround = i;
				best_put_time=put_time[curnum];
				memcpy(best_influenced_num_round, influenced_num_round, sizeof(best_influenced_num_round));
			}
			else if(bestseed_thisround == -1 || min_overthres<thres1){
				if(min_overthres==1e5){
					min_overthres=peak;
					bestseed_thisround = i;
					best_put_time = put_time[curnum];
				}
				else if(min_overthres < peak){
					min_overthres=peak;
					bestseed_thisround = i;
					best_put_time = put_time[curnum];
				}
			}
		}//for(int i=0; i<seednum; i++)
		if(bestseed_thisround == -1){
			printf("no proper seed!!!!!\n");
			for(int i=0; i<initnum; i++)
				if(be_put[i]==0){
					be_put[i]=1;
					bestseed_thisround=i;
					break;
				}
		}
		//add the best current seed to the ans

		double max_heap=0;
		for(int j=tmp_begin_round; j < tmp_begin_round+35; j++){
			//printf("%lf\n",influenced_num_round[j]);
			if(max_heap < best_influenced_num_round[j]){
				max_heap = best_influenced_num_round[j];
				begin_round=j;
			}
		}
		printf("begin_round is %d\n peak is : %.3lf", begin_round, max_heap);
		//the new peak is more previous than the put time of the current seed
		//it mean even the best seed doesn't have the good influence
		if(begin_round < best_put_time){
			best_put_time = put_time[curnum-1];
			begin_round=best_put_time;
			printf("these seeds have no influence\n");
		}
		last_effect=seed_effect[bestseed_thisround];
		best_put_order[curnum] = seed[bestseed_thisround];
		be_put[bestseed_thisround] = 1;
		put_time[curnum]=best_put_time;
		printf("----------%d round seed is %d, put time is %d\n", curnum, best_put_order[curnum], put_time[curnum]);
		curnum++;
	}//while(curnum < seednum)
	for(int i=0; i<seednum; i++)
		seed[i] = best_put_order[i];
}
