#include <iostream>
#include <cstdio>
#include <cstring>
#include <algorithm>

#define maxx 90000

using namespace std;

struct effect{
	char id[20];
	double effect_times;
}user[maxx];

bool cmp(effect a, effect b){
	return a.effect_times>b.effect_times;
}

int main(){
	char s[20];
	double d;
	FILE *read,*write;

	read=fopen("effect_times.txt","r");
	write=fopen("effect_times_sort.txt","w");

	int num=0;
	while(fscanf(read,"%s %lf",s, &d) !=EOF){
		strcpy(user[num].id,s);
		user[num++].effect_times=d;
	}
	sort(user,user+num,cmp);

	for(int i=0;i<num;i++)
		fprintf(write,"%s %lf\n",user[i].id, user[i].effect_times);

	fclose(read);
	fclose(write);
	return 0;
}