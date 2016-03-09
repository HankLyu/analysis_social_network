#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <algorithm>

using namespace std;

const int peopleSize = 200000;
const int seedsNumber = 5;
const int simulateTimes = 1000;
const int basicAbilityOfrSeed = 300;
const int basicInfluenceNumber = 60;
const int maxRound = 100;
const int errOfBasicInfluenceNumberOfBasicInfluenceNumber = 3;

struct Friend {
	int id;
	double closeness;
};

struct social {
	int id;
	bool isActived; //if 1 this is be infected, if 0, otherwise
	int round_time;
	double expect;
	vector<Friend> friends;
};

social user[peopleSize];
int exist[peopleSize];
int maxId; //maxId to record the max id

double take_random();
bool is_all_init_put(int seed[], int seednum);
void choiceSeedByRandom(int seed[]);
void choice_seed(int by_choice[], int num, int seed[]);
void run_result(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum);
void run_puttime(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum, int startnum);
void greedy(int seed[], int putTimeOfSeed[], int numOfSeed,
	int seedBeEffectedBeforePutTimes[], double numberOfNodeBeEffectedForEachRound[]);
void readFileAndBuildFrendRelation(FILE *edgeFile);

int main(int argc, char* argv[]) {
	int a, b;
	double c;
	int seed[seedsNumber];
	int influence_times[peopleSize];
	double influenced_num_round[seedsNumber * 20] = {0}, sum = 0;
	FILE *edgeFile, *outputFile;
	time_t start_time, finish_time;

	edgeFile = fopen(argv[1], "r");
	outputFile = fopen("greedy_put.txt", "w");
	memset(user, 0, sizeof (user));

	readFileAndBuildFrendRelation(edgeFile);

	srand(time(NULL));

	choiceSeedByRandom(seed);
	
	int seed_be_effected[seedsNumber] = {0};
	int put_time[seedsNumber] = {0}; //the round should be put
	start_time = time(NULL);
	greedy(seed, put_time, seedsNumber, seed_be_effected, influenced_num_round);
	memset(seed_be_effected, 0, sizeof (seed_be_effected));
	run_result(seed, put_time, influenced_num_round, seed_be_effected, seedsNumber);
	finish_time = time(NULL);
	std::cout << "start time " << start_time << endl << "end time " << finish_time << endl;
	std::cout << "the greedy time is " << (finish_time - start_time) << endl;
	fprintf(outputFile, "The greedy run time is %ld\n", (finish_time - start_time));
	memset(seed_be_effected, 0, sizeof (seed_be_effected));
	//print the result
	//printf("init node:\n");
	//for(int i=0;i<seedsNumber;i++)
	//	printf("%4d\n",seed[i]);
	int basicInfluenceNumber_up = 0, thres2_up = 0;
	fprintf(outputFile, "Greedy:\nThe initial node and its num of friends\n");
	for (int i = 0; i < seedsNumber; i++)
		fprintf(outputFile, "%4d\t%4lu\t%.3lf\n", seed[i], user[seed[i]].friends.size()
		, (double) user[seed[i]].expect);
	fprintf(outputFile, "the order of seeds\n");
	for (int i = 0; i < seedsNumber; i++)
		fprintf(outputFile, "%4d, ", seed[i]);
	fprintf(outputFile, "\nput round\n");
	for (int i = 0; i < seedsNumber; i++)
		fprintf(outputFile, "%3d, ", put_time[i]);
	fprintf(outputFile, "\n");
	bool upto_thres = false;
	for (int k = 0; influenced_num_round[k] > 0.0; k++) { //caulate the average of each round
		if (influenced_num_round[k] > basicInfluenceNumber) upto_thres = true;
		if (upto_thres) {
			//printf("tmp_num_round %d\n", tmp_num_round);
			if (influenced_num_round[k] > basicInfluenceNumber) basicInfluenceNumber_up++;
			else break;
		}
	}
	for (int i = 0; influenced_num_round[i] > 0.0; i++) { //caulate the average of each round
		sum += influenced_num_round[i];
	}
	fprintf(outputFile, "%4d up:\t%3d\n", basicInfluenceNumber, basicInfluenceNumber_up);
	fprintf(outputFile, "the average num: \n%.3lf\n", sum);
	fprintf(outputFile, "average round\n");
	for (int i = 0; influenced_num_round[i] > 1e-7; i++)
		fprintf(outputFile, "%.3lf\n", influenced_num_round[i]);

	fclose(edgeFile);
	fclose(outputFile);
	return 0;
}

void greedy(int seed[], int putTimeOfSeed[], int numOfSeed,
	int seedBeEffectedBeforePutTimes[], double numberOfNodeBeEffectedForEachRound[]) {
	int nthSeed = 0;
	bool thisSeedHasBeenPut[seedsNumber] = {false};
	int numberOfCurrentEffectiveRound = 0, thisSeedHasEffectiveRound[seedsNumber] = {0};
	double bestInfluenceResultForEachRound[seedsNumber * 20];
	int bestPutOrderForSeeds[seedsNumber], peakRound = 0, tmpPeakRound;
	int bestPutTimeOfNextSeed;
	
	while (nthSeed < numOfSeed) {
		bool isThisSeedHasInfluence = false;
		int bestPeak = 1e5; //to find the  best seed having the min peak for this order to make sure the wave is not too high
		int peakOfThisSeed;
		int bestCurrentSeed = -1;

		memset(thisSeedHasEffectiveRound, 0, sizeof (thisSeedHasEffectiveRound));
		for (int i = 0; i < numOfSeed; i++) {
			if (thisSeedHasBeenPut[i] == true) continue;

			bestPutOrderForSeeds[nthSeed] = seed[i];
			run_puttime(bestPutOrderForSeeds, putTimeOfSeed, numberOfNodeBeEffectedForEachRound, seedBeEffectedBeforePutTimes, nthSeed + 1, nthSeed);
			tmpPeakRound = peakRound;
			//chose the peak
			for (int j = tmpPeakRound; j < tmpPeakRound + 35; j++) {
				peakOfThisSeed = max(peakOfThisSeed, (int) numberOfNodeBeEffectedForEachRound[j]);
			}
			
			//caulate the average of each round in continus
			bool startToCountEffectiveRound = false;
			for (int k = 0; numberOfNodeBeEffectedForEachRound[k] > 0.0; k++) { 
				if (numberOfNodeBeEffectedForEachRound[k] > basicInfluenceNumber) 
					startToCountEffectiveRound = true;
				if (startToCountEffectiveRound) {
					if (numberOfNodeBeEffectedForEachRound[k] > basicInfluenceNumber) 
						thisSeedHasEffectiveRound[i]++;
					else break;
				}
				//printf("%d: %.1lf\n", i, influenced_num_round[k]);
			}
			//printf("seed %d max over %d\n", seed[i], peak);
			if (peakOfThisSeed < bestPeak && peakOfThisSeed > basicInfluenceNumber
				&& thisSeedHasEffectiveRound[i] > numberOfCurrentEffectiveRound + 1) { //check whether the current seed is good
				bestPeak = peakOfThisSeed; //to sure the new seed just over the threshold a bit
				bestCurrentSeed = i;
				bestPutTimeOfNextSeed = putTimeOfSeed[nthSeed];
				isThisSeedHasInfluence = true;
				memcpy(bestInfluenceResultForEachRound,
					numberOfNodeBeEffectedForEachRound,
					sizeof (bestInfluenceResultForEachRound));
			}
			printf("the current best seed is: %d  peak: %d last_effect: %d, this seed effected %d\n"
				, seed[bestCurrentSeed], bestPeak, numberOfCurrentEffectiveRound, thisSeedHasEffectiveRound[i]);
		}//for(int i=0; i<seednum; i++)
		if (bestCurrentSeed == -1) {
			printf("no proper seed!!!!!\n");
			for (int i = 0; i < seedsNumber; i++)
				if (thisSeedHasBeenPut[i] == false) {
					thisSeedHasBeenPut[i] = true;
					bestCurrentSeed = i;
					bestPutTimeOfNextSeed = putTimeOfSeed[nthSeed - 1];
					break;
				}
			isThisSeedHasInfluence = false;
		}
		double new_peak = 0;
		for (int j = tmpPeakRound; j < tmpPeakRound + 35; j++) {
			//printf("%lf\n",influenced_num_round[j]);
			if (new_peak < bestInfluenceResultForEachRound[j]) {
				new_peak = bestInfluenceResultForEachRound[j];
				peakRound = j;
			}
		}
		printf("begin_round is %d\n peak is : %.3lf", peakRound, new_peak);
		if (isThisSeedHasInfluence = true)
			numberOfCurrentEffectiveRound = thisSeedHasEffectiveRound[bestCurrentSeed];
		else {
			bestPutTimeOfNextSeed = putTimeOfSeed[nthSeed - 1];
			peakRound = bestPutTimeOfNextSeed;
		}
		bestPutOrderForSeeds[nthSeed] = seed[bestCurrentSeed];
		thisSeedHasBeenPut[bestCurrentSeed] = true;
		putTimeOfSeed[nthSeed] = bestPutTimeOfNextSeed;
		printf("----------%d round seed is %d, put time is %d\n", nthSeed, bestPutOrderForSeeds[nthSeed], putTimeOfSeed[nthSeed]);
		nthSeed++;
	}//while(nthSeed < seednum)
	for (int i = 0; i < numOfSeed; i++)
		seed[i] = bestPutOrderForSeeds[i];
}

void run_puttime(int seed[], int put_time[], double influenced_num_round[],
	int seedBeEffectedBeforePut[], int seednum, int startnum) {
	double best_influenced_num_round[seedsNumber * 20];
	for (int i = startnum; i < seednum; i++) {
		if (i == 0) {
			put_time[0] = 0;
			printf("choose first seed: %d\n", seed[0]);
			run_result(seed, put_time, influenced_num_round, seedBeEffectedBeforePut, i + 1);
			continue;
		}
		int max_num_round = 0, best_put_time = put_time[i - 1], max_thres = 0;
		int tmp_num_round;
		printf("i=%d\n", i);
		for (int j = put_time[i - 1]; j < put_time[i - 1] + 15; j++) {
			put_time[i] = j;
			run_result(seed, put_time, influenced_num_round, seedBeEffectedBeforePut, i + 1);
			tmp_num_round = 0;
			bool upto_thres = false;
			for (int k = 0; influenced_num_round[k] > 0.0; k++) { //caulate the average of each round
				if (influenced_num_round[k] > basicInfluenceNumber) upto_thres = true;
				if (upto_thres) {
					//printf("tmp_num_round %d\n", tmp_num_round);
					if (influenced_num_round[k] >
						basicInfluenceNumber + errOfBasicInfluenceNumberOfBasicInfluenceNumber)
						tmp_num_round++;
					else break;
				}
			}
			//printf("%d %d %d\n", i, j, tmp_num_round);
			if (max_num_round < tmp_num_round) {
				max_num_round = tmp_num_round;
				best_put_time = j;
				memcpy(best_influenced_num_round, influenced_num_round, sizeof (best_influenced_num_round));
			}
		}//for(int j=put_time[i]+1; j<put_time[i]+15;j++)
		put_time[i] = best_put_time;
		printf("this seed %d: best_put_time is %d, round: %d", seed[i], put_time[i], max_num_round);
		memcpy(influenced_num_round, best_influenced_num_round, sizeof (best_influenced_num_round)); //return the best of the influence of this seed
	}//for(int i=1; i<seednum; i++)
}

void run_result(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum) {
	bool put_activenode[seedsNumber + 5] = {0}; //to put active node for the some round
	int next_seed; //next_seed record the  which round is next round 	
	int times_result_num[seedsNumber * 20] = {0};
	queue<int>infl; //to do bfs
	//The follow is run the experence
	//it is round (simulateTimes) times to record the sum of each experence result
	//then average the result
	for (int t = 0; t < simulateTimes; t++) {
		while (!infl.empty()) infl.pop();
		for (int i = 0; i < maxId; i++) { //initial each node
			user[i].isActived = 0;
			user[i].round_time = -1;
		}
		next_seed = 0;
		memset(put_activenode, 0, sizeof (put_activenode));
		int tmp, tmp_round = 0; //tmp_round record round_time of queue.front
		while (!infl.empty() || !is_all_init_put(seed, seednum)) {
			//for each round to put the act node once, and check the node
			//has been influenced. if yes record this, no put it into queue
			//////////////////////////////////////////////////////////////////////////////////
			if (put_time[next_seed] == tmp_round && next_seed < seednum && put_activenode[next_seed] == false) {
				put_activenode[next_seed] = true;
				if (user[seed[next_seed]].isActived == 0) {
					infl.push(seed[next_seed]);
					user[seed[next_seed]].isActived = 1;
					user[seed[next_seed]].round_time = tmp_round;
				} else if (next_seed < seednum) {
					seed_be_effected[next_seed]++;
				}
				next_seed++;
			}
			/////////////////////////////////////////////////////////////////////////////////
			if (!infl.empty()) {
				tmp = infl.front();
				infl.pop();
				tmp_round = user[tmp].round_time;
				times_result_num[tmp_round]++;
			} else {
				tmp_round = put_time[next_seed];
				continue;
			}
			//printf("tmp_round=%d %d %d\n",tmp_round,next_seed,put_activenode[next_seed]);
			int friendnum = user[tmp].friends.size();
			for (int i = 0, j; i < friendnum; i++) {
				j = user[tmp].friends[i].id;
				if (!user[j].isActived && take_random() < user[tmp].friends[i].closeness) {
					infl.push(j);
					user[j].round_time = user[tmp].round_time + 1;
					user[j].isActived = 1;
				}
			}
		}//while(!infl.empty())
	}//for(simulateTimes)
	for (int i = 0; i < seedsNumber * 20; i++) { //caulate the average of each round
		influenced_num_round[i] = times_result_num[i] / (double) simulateTimes;
	}
}

void readFileAndBuildFrendRelation(FILE *edgeFile) {
	maxId = 0;
	int lPerson, rPerson;
	double closeness;
	Friend inputfriend;
	while (fscanf(edgeFile, "%d %d %lf", &lPerson, &rPerson, &closeness) != EOF) {
		user[lPerson].id = lPerson;
		inputfriend.id = rPerson;
		inputfriend.closeness = closeness;
		user[lPerson].friends.push_back(inputfriend);
		user[lPerson].expect += closeness;
		maxId = max(maxId, lPerson);
		exist[lPerson]++;
	}
	printf("read file done\n");
}

double take_random() {
	double result = (double) rand() / (RAND_MAX + 1.0);
	return result;
}

bool is_all_init_put(int seed[], int seednum) {
	for (int i = 0; i < seednum; i++)
		if (user[seed[i]].isActived == 0)
			return false;
	return true;
}

void choiceSeedByRandom(int seed[]) {
	double sum, tmp_influence[seedsNumber * 20];
	int tmp_seed[5], tmp_puttime[5], tmp_effect[5];
	for (int i = 0, j; i < seedsNumber; i++) { //choose seed
		int findTimes = 100;
		while (findTimes--) {
			j = rand() % maxId;
			if (user[j].isActived == 1 || exist[j] == 0) continue;
			tmp_seed[0] = j;
			tmp_puttime[0] = 0;
			run_result(tmp_seed, tmp_puttime, tmp_influence, tmp_effect, 1);
			sum = 0;
			for (int k = 0; tmp_influence[k] > 0.0 && k < seedsNumber * 20; k++) {
				sum += tmp_influence[k];
			}
			if (sum > basicAbilityOfrSeed)
				break;
		}
		if (findTimes == 0) {
			printf("no proper seed\n");
		} else {
			printf("seed: %d id: %d effected num: %lf\n", i, j, sum);
		}
		//if this user is selected or this user is not existed
		//choose new user again
		seed[i] = j;
		//user[j].active=1;
	}
}

void choice_seed(int by_choice[], int num, int seed[]) {
	bool chose[seedsNumber] = {0};
	int randnum;
	for (int i = 0; i < num; i++) {
		/*do{	//this comment is to do random order for seed
			randnum = rand()%num;
		}
		while(chose[randnum] == true);
		chose[randnum]=true;
		 */
		//printf("%d\n", randnum);
		randnum = i;
		seed[randnum] = by_choice[i];
	}
}




