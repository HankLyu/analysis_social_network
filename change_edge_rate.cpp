#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>

using namespace std;

int main(int argv,char *argc[]){
	FILE *inEdge, *outEdge, *inRate;
	inEdge=fopen(argc[1],"r");
	outEdge=fopen(argc[2],"w");
	inRate=fopen("prop_dblp_8020","r");
	char  a[10],b[10],c[30];
	int random;
	srand(time(NULL));
	int rate;
	while(fscanf(inEdge,"%s %s",a,b) != EOF){
		fscanf(inRate,"%s",c);
		fprintf(outEdge, "%s %s %s\n",a,b,c);
		/*random=rand()%3;
		if(random==0)	fprintf(outEdge,"0.1\n");
		if(random==1)	fprintf(outEdge,"0.01\n");
		if(random==2)	fprintf(outEdge,"0.001\n");
		*/
	}
	fclose(inEdge);
	fclose(outEdge);
	fclose(inRate);
	return 0;
}