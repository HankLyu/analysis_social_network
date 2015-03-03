#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <algorithm>

#define maxx 90000
#define runtimes 10000
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
bool is_all_init_put(int seed[], int seednum);
void random_choice_seed(int seed[]);
void choice_seed(int by_choice[],int num, int seed[]);
void run_result(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum);
void run_puttime(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum, int startnum);
void greedy(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum);

int main(int argc,char* argv[]){
	int a,b;
	double c;
	int seed[100];
	int influence_times[maxx];
	double influenced_num_round[100]={0},sum=0;
	FILE *read_edge,*out;
	time_t start_time, finish_time;

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

	//random_choice_seed(seed);
	//friend_choice_seed();
	//9881, 30635, 8932

	//int by_choice[]={16751, 14014, 10464, 26970, 7781, 14625};
	int by_choice[]={7781, 14625, 6751, 26970, 14014, 10464};
	choice_seed(by_choice, initnum, seed);
	for(int i=0;i<initnum;i++)
		printf("%d ",seed[i]);
	printf("\n");

	int seed_be_effected[10]={0};		//count it is active node but it have been effected times
	int put_time[10];	//the round should be put

	start_time = time(NULL);
	greedy(seed, put_time, influenced_num_round, seed_be_effected, initnum);
	//run_puttime(seed, put_time, influenced_num_round, seed_be_effected, initnum, 0);
	run_result(seed, put_time, influenced_num_round, seed_be_effected, initnum);
	finish_time = time(NULL);
	std::cout<<"start time "<<start_time<<endl << "end time "<< finish_time<<endl;
	std::cout<< "the finish time is "<<(finish_time - start_time)<<endl;
	fprintf(out,"The estimate run time is %d\n", (finish_time - start_time));
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
	fprintf(out,"put round\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%3d\t",put_time[i]);
	fprintf(out,"\ninit node be influeced before active times\n");
	for(int i=0;i<initnum;i++)
		fprintf(out,"%.3lf\t", (double)seed_be_effected[i]/(double)runtimes);
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
	bool chose[initnum]={0};
	int randnum;
	for(int i=0;i<num;i++){
		/*do{	//this comment is to do random order for seed
			randnum = rand()%num;
		}
		while(chose[randnum] == true);
		chose[randnum]=true;
		*/
		//printf("%d\n", randnum);
		randnum=i;
		seed[randnum]=by_choice[i];
	}
}

void run_result(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum){
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
	for(int i=0;times_result_num[i]!=0 || i<put_time[seednum];i++){		//caulate the average of each round
		influenced_num_round[i] = times_result_num[i]/(double)runtimes;
	}
}

void run_puttime(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum, int startnum){
	for(int i=startnum; i<seednum; i++){
		if(i == 0){
			put_time[0]=0;
			run_result(seed, put_time, influenced_num_round, seed_be_effected, i+1);
			continue;
		}
		int max_num_round=0, best_put_time=0, max_thres=0;
		int tmp_num_round;

        // last_node_infl algorithm core
        int last_round_under_thres;
        run_result(seed, put_time, influenced_num_round, seed_be_effected, i);
        bool upto_thres=false;
        for(int j=put_time[i-1]+1;influenced_num_round[j] != 0; j++){
            if(influenced_num_round[j] > thres1)	upto_thres=true;
            if(upto_thres){
            	if(influenced_num_round[j] < thres1){
            		last_round_under_thres = j-1;
            		break;
            	}
            }
            
        	
        }

		printf("i=%d\n",i);
		for(int j=put_time[i-1]+1; j<last_round_under_thres;j++){
			put_time[i]=j;
			run_result(seed, put_time, influenced_num_round, seed_be_effected, i+1);
			tmp_num_round=0;
			bool upto_thres=false;
			for(int k=0;influenced_num_round[k]> 0.0; k++){		//caulate the average of each round
				if(influenced_num_round[k] > thres1){
					upto_thres=true;
					tmp_num_round++;
				}		
				if(upto_thres && influenced_num_round[k]<thres1){
					break;
				}
			}
			if(max_num_round < tmp_num_round){
				max_num_round = tmp_num_round;
				best_put_time = j;
			}
		}//for(int j=put_time[i]+1; j<put_time[i]+15;j++)
		put_time[i]=best_put_time;
		printf("best_put_time is %d\n", put_time[i]);
	}//for(int i=1; i<seednum; i++)
}

void greedy(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum){
	int curnum=0, be_put[initnum]={0};
	int best_put_order[initnum];
	int best_put_time;	//record the time of the current seed
	for(int i=0;i<curnum;i++){	//if the order of the part seeds is sure, change the "curnum" valure
		be_put[i]=1;
		best_put_order[i]=seed[i];
	}
	while(curnum < seednum){
		int min_overthres=1e5;	//record best seed for this order to make sure the wave is not too high
		int bestseed_thisround=-1;	//to record which seed is best in this round

		for(int i=0; i<seednum; i++){ //to
			int max_overthres=0;	//record for each point which is max
			if(be_put[i] == 1)	continue;

			best_put_order[curnum] = seed[i];
			run_puttime(best_put_order, put_time, influenced_num_round, seed_be_effected, curnum+1, curnum);
			for(int j=put_time[curnum]; j < put_time[curnum]+35; j++){	//check the influencu wave
				//printf("%lf\n",influenced_num_round[j]);
				if(influenced_num_round[j]>thres1){
					max_overthres=max(max_overthres, (int)influenced_num_round[j]);
				}
			}
			//printf("seed %d max over %d\n", seed[i], max_overthres);
			if(max_overthres < min_overthres && max_overthres>thres1){	//check whether the current seed is good
				min_overthres=max_overthres;	//to sure the new seed just over the threshold a bit
				bestseed_thisround = i;
				best_put_time=put_time[curnum];
			}
		}//for(int i=0; i<seednum; i++)
		if(bestseed_thisround == -1)	printf("error!!!!!\n");
		best_put_order[curnum] = seed[bestseed_thisround];
		be_put[bestseed_thisround] = 1;
		put_time[curnum]=best_put_time;
		printf("----------%d round is %d\n", curnum, best_put_order[curnum]);
		curnum++;
	}//while(curnum < seednum)
	for(int i=0; i<seednum; i++)
		seed[i] = best_put_order[i];
}