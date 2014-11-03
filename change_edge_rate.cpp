#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

using namespace std;

int main(int argv,char *argc[]){
	FILE *inEdge,*outEdge;
	inEdge=fopen(argc[1],"r");
	outEdge=fopen(argc[2],"w");
	char  a[10],b[10],c[10];
	int random;
	srand(time(NULL));
	int rate;
	while(fscanf(inEdge,"%s %s",a,b,c) != EOF){
		fprintf(outEdge, "%s %s ",a,b);
		random=rand()%3;
		if(random==0)	fprintf(outEdge,"0.1\n");
		if(random==1)	fprintf(outEdge,"0.01\n");
		if(random==2)	fprintf(outEdge,"0.001\n");
		
	}
	return 0;
}