#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <algorithm>

using namespace std;

struct edge{
	int x, y;
};
bool edge_sort(edge a, edge b){
	return (a.x < b.x ) || 
			(a.x == b.x && a.y < b.y);
}

int main(int argc, char* argv[]){
	FILE *in,*out;
	int edgenum=0;
	srand(time(NULL));
	in=fopen(argv[1],"r");
	out=fopen(argv[2],"w");

	vector<edge>edge_new;
	edge edge_tmp;
	int a,b;
	while(fscanf(in,"%d %d",&a,&b)!=EOF){
		if(a>b)	continue;
		if(rand()%2 ==0){
			edge_tmp.x=a;
			edge_tmp.y=b;
		}
		else{
			edge_tmp.x=b;
			edge_tmp.y=a;
		}
		edge_new.push_back(edge_tmp);
		edgenum++;
	}
	sort(edge_new.begin(), edge_new.end(), edge_sort);
	for(int i=0;i<edgenum;i++)
		fprintf(out,"%d	%d\n",edge_new[i].x, edge_new[i].y);
	fclose(in);
	fclose(out);
	return 0;
}