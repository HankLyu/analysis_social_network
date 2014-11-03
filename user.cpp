#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <algorithm>

#define maxx 90000
#define runtimes 5000
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

bool cmp(social a, social b){
	return a.expect>b.expect;
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
	out=fopen(argv[2],"w");
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
	nummax++;

	sort(user,user+nummax,cmp);
	for(int i=0;i<nummax;i++)
		if(user[i].friends.size()!=0){
			fprintf(out,"%5d %4d    %3.3lf\n",
				user[i].id,user[i].friends.size(),user[i].expect);
		}
	return 0;
}
