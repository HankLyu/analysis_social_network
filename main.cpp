#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <queue>
#include <algorithm>

using namespace std;

const int PeopleSize = 200000;
const int SeedsNumber = 5;
const int SimulateTimes = 1000;
const int BasicAbilityOfrSeed = 300;
const int BasicInfluenceNumber = 60;
const int MaxRound = 100;
const int ErrOfBasicInfluenceNumberOfBasicInfluenceNumber = 3;

struct Friend{
	int id;
	double closeness;
};

struct social{
	int id;
	bool isActived; //if 1 this is be infected, if 0, otherwise
	int round_time;
	double expect;
	vector<Friend> friends;
};

social user[PeopleSize];
int exist[PeopleSize];
int maxId; //maxId to record the max id

double take_random();
void choiceSeedByRandom(int seed[]);
void greedy(int seed[], int putTimeOfSeed[], int seedsNumber,
	int timesOfSeedBeEffectedBeforePut[], double numberOfNodeBeEffectedForEachRound[]);
void readFileAndBuildFrendRelation(FILE *edgeFile);
void outputResultToFile(FILE *outputFile, int seed[], int putTimeOfSeed[], int seedsNumber,
	double numberOfNodeBeEffectedForEachRound[], time_t startTime, time_t endTime);

int main(int argc, char* argv[]){
	int seed[SeedsNumber];
	int influence_times[PeopleSize];
	double numberOfNodeBeEffectedForEachRound[SeedsNumber * 20] = {0};
	int timesOfSeedBeEffectedBeforePut[SeedsNumber] = {0};
	int putTimeOfSeed[SeedsNumber] = {0};
	FILE *edgeFile, *outputFile;
	time_t startTime, endTime;

	edgeFile = fopen(argv[1], "r");
	outputFile = fopen("greedy_put.txt", "w");
	memset(user, 0, sizeof(user));

	readFileAndBuildFrendRelation(edgeFile);
	srand(time(NULL));
	choiceSeedByRandom(seed);


	startTime = time(NULL);
	greedy(seed, putTimeOfSeed, SeedsNumber, timesOfSeedBeEffectedBeforePut, numberOfNodeBeEffectedForEachRound);
	endTime = time(NULL);

	std::cout << "the greedy time is " << (endTime - startTime) << endl;

	outputResultToFile(outputFile, seed, putTimeOfSeed, SeedsNumber,
		numberOfNodeBeEffectedForEachRound, startTime, endTime);

	fclose(edgeFile);
	fclose(outputFile);
	return 0;
}

void run_result(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum);
void findPutTimeForFixOrderSeeds(int seed[], int put_time[], double influenced_num_round[],
	int seed_be_effected[], const int seednum, const int startnum);
int countEffectiveRound(double numberOfNodeBeEffectedForEachRound[]);

void greedy(int seed[], int putTimeOfSeed[], int seedsNumber,
	int timesOfSeedBeEffectedBeforePut[], double numberOfNodeBeEffectedForEachRound[]){
	int nthSeed = 0;
	bool isThisSeedHasBeenPut[seedsNumber] = {false};
	int numberOfCurrentEffectiveRound = 0;
	double bestInfluenceResultForEachRound[seedsNumber * 20];
	int bestPutOrderForSeeds[seedsNumber], peakRound = 0, tmpPeakRound;
	int bestPutTimeOfNextSeed;

	while(nthSeed < seedsNumber){
		bool isThisSeedHasInfluence = false;
		int bestPeak = 1e5; //to find the  best seed having the min peak for this order to make sure the wave is not too high
		int peakOfTestSeed;
		int bestCurrentSeed = -1;

		// Find the best seed to put
		int numOfEffectiveRoundOfSeed[seedsNumber] = {0};
		memset(numOfEffectiveRoundOfSeed, 0, sizeof(numOfEffectiveRoundOfSeed));
		for(int testSeed = 0; testSeed < seedsNumber; testSeed++){
			if(isThisSeedHasBeenPut[testSeed] == true) continue;

			bestPutOrderForSeeds[nthSeed] = seed[testSeed];
			findPutTimeForFixOrderSeeds(bestPutOrderForSeeds, putTimeOfSeed, numberOfNodeBeEffectedForEachRound, timesOfSeedBeEffectedBeforePut, nthSeed + 1, nthSeed);
			tmpPeakRound = peakRound;
			// Find the peak
			peakOfTestSeed = 0;
			for(int j = tmpPeakRound; j < tmpPeakRound + 35; j++){
				peakOfTestSeed = max(peakOfTestSeed, (int) numberOfNodeBeEffectedForEachRound[j]);
			}
			numOfEffectiveRoundOfSeed[testSeed] = countEffectiveRound(numberOfNodeBeEffectedForEachRound);

			// To check whether this current seed is the proper seed be putted at next time
			if(peakOfTestSeed < bestPeak && peakOfTestSeed > BasicInfluenceNumber
				&& numOfEffectiveRoundOfSeed[testSeed] > numberOfCurrentEffectiveRound + 1){
				bestPeak = peakOfTestSeed;
				bestCurrentSeed = testSeed;
				bestPutTimeOfNextSeed = putTimeOfSeed[nthSeed];
				isThisSeedHasInfluence = true;
				memcpy(bestInfluenceResultForEachRound,
					numberOfNodeBeEffectedForEachRound,
					sizeof(bestInfluenceResultForEachRound));
			}
			printf("\nthe current best seed is: %d ", seed[bestCurrentSeed]);
			printf("peak: %d \n", peakOfTestSeed);
			printf("numberOfCurrentEffectiveRound: %d ", numberOfCurrentEffectiveRound);
			printf("this seed effected %d\n", numOfEffectiveRoundOfSeed[testSeed]);
		}//for(int i=0; i<seednum; i++)
		if(bestCurrentSeed == -1){
			printf("\nNo proper seed!!!!!\n");
			for(int i = 0; i < seedsNumber; i++)
				if(isThisSeedHasBeenPut[i] == false){
					isThisSeedHasBeenPut[i] = true;
					bestCurrentSeed = i;
					bestPutTimeOfNextSeed = putTimeOfSeed[nthSeed - 1];
					break;
				}
			isThisSeedHasInfluence = false;
		}
		int numOfEffectiveRoundBestSeed = countEffectiveRound(numberOfNodeBeEffectedForEachRound);

		double newPeak = 0;
		for(int j = tmpPeakRound; j < tmpPeakRound + 35; j++){
			//printf("%lf\n",influenced_num_round[j]);
			if(newPeak < bestInfluenceResultForEachRound[j]){
				newPeak = bestInfluenceResultForEachRound[j];
				peakRound = j;
			}
		}
		if(isThisSeedHasInfluence = true)
			numberOfCurrentEffectiveRound = numOfEffectiveRoundBestSeed;
		else{
			bestPutTimeOfNextSeed = putTimeOfSeed[nthSeed - 1];
			peakRound = bestPutTimeOfNextSeed;
		}
		bestPutOrderForSeeds[nthSeed] = seed[bestCurrentSeed];
		isThisSeedHasBeenPut[bestCurrentSeed] = true;
		putTimeOfSeed[nthSeed] = bestPutTimeOfNextSeed;
		printf("begin_round is %d\n peak is : %.3lf\n", peakRound, newPeak);
		printf("%d round seed is %d, put time is %d\n\n\n", nthSeed, bestPutOrderForSeeds[nthSeed], putTimeOfSeed[nthSeed]);
		nthSeed++;
	}//while(nthSeed < seednum)
	for(int i = 0; i < seedsNumber; i++)
		seed[i] = bestPutOrderForSeeds[i];
	for(int i = 0; i < seedsNumber; i++){
		timesOfSeedBeEffectedBeforePut[i] = 0;
	}
	run_result(seed, putTimeOfSeed, numberOfNodeBeEffectedForEachRound, timesOfSeedBeEffectedBeforePut, seedsNumber);
}

void findPutTimeForFixOrderSeeds(int seed[], int putTimeOfSeed[], double numberOfNodeBeEffectedForEachRound[],
	int seedBeEffectedBeforePut[], const int seedNum, const int startFindNum){
	
	double bestResultForEachRound[SeedsNumber * 20];
	const int TestRoundRange = 15;
	
	for(int i = startFindNum; i < seedNum; i++){
		if(i == 0){
			putTimeOfSeed[0] = 0;
			printf("choose first seed: %d\n", seed[0]);
			run_result(seed, putTimeOfSeed, numberOfNodeBeEffectedForEachRound, seedBeEffectedBeforePut, i + 1);
			continue;
		}
		int max_num_round = 0, best_put_time = putTimeOfSeed[i - 1], max_thres = 0;
		int tmp_num_round;
		printf("i=%d\n", i);
		for(int testPutTime = putTimeOfSeed[i - 1];testPutTime < putTimeOfSeed[i - 1] + TestRoundRange; testPutTime++){
			putTimeOfSeed[i] = testPutTime;
			run_result(seed, putTimeOfSeed, numberOfNodeBeEffectedForEachRound, seedBeEffectedBeforePut, i + 1);
			tmp_num_round = 0;
			bool upto_thres = false;
			for(int k = 0; numberOfNodeBeEffectedForEachRound[k] > 0.0; k++){ //caulate the average of each round
				if(numberOfNodeBeEffectedForEachRound[k] > BasicInfluenceNumber) upto_thres = true;
				if(upto_thres){
					//printf("tmp_num_round %d\n", tmp_num_round);
					if(numberOfNodeBeEffectedForEachRound[k] >
						BasicInfluenceNumber + ErrOfBasicInfluenceNumberOfBasicInfluenceNumber)
						tmp_num_round++;
					else break;
				}
			}
			//printf("%d %d %d\n", i, j, tmp_num_round);
			if(max_num_round < tmp_num_round){
				max_num_round = tmp_num_round;
				best_put_time = testPutTime;
				memcpy(bestResultForEachRound, numberOfNodeBeEffectedForEachRound, sizeof(bestResultForEachRound));
			}
		}//for(int j=put_time[i]+1; j<put_time[i]+15;j++)
		putTimeOfSeed[i] = best_put_time;
		printf("this seed %d: best_put_time is %d, round: %d,   ", seed[i], putTimeOfSeed[i], max_num_round);
		memcpy(numberOfNodeBeEffectedForEachRound, bestResultForEachRound, sizeof(bestResultForEachRound)); //return the best of the influence of this seed
	}//for(int i=1; i<seednum; i++)
}

bool is_all_init_put(int seed[], int seednum);

void run_result(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum){
	bool put_activenode[SeedsNumber + 5] = {0}; //to put active node for the some round
	int next_seed; //next_seed record the  which round is next round 	
	int times_result_num[SeedsNumber * 20] = {0};
	queue<int>infl; //to do bfs
	//The follow is run the experence
	//it is round (simulateTimes) times to record the sum of each experence result
	//then average the result
	for(int t = 0; t < SimulateTimes; t++){
		while(!infl.empty()) infl.pop();
		for(int i = 0; i < maxId; i++){ //initial each node
			user[i].isActived = 0;
			user[i].round_time = -1;
		}
		next_seed = 0;
		memset(put_activenode, 0, sizeof(put_activenode));
		int tmp, tmp_round = 0; //tmp_round record round_time of queue.front
		while(!infl.empty() || !is_all_init_put(seed, seednum)){
			//for each round to put the act node once, and check the node
			//has been influenced. if yes record this, no put it into queue
			//////////////////////////////////////////////////////////////////////////////////
			if(put_time[next_seed] == tmp_round && next_seed < seednum && put_activenode[next_seed] == false){
				put_activenode[next_seed] = true;
				if(user[seed[next_seed]].isActived == 0){
					infl.push(seed[next_seed]);
					user[seed[next_seed]].isActived = 1;
					user[seed[next_seed]].round_time = tmp_round;
				} else if(next_seed < seednum){
					seed_be_effected[next_seed]++;
				}
				next_seed++;
			}
			/////////////////////////////////////////////////////////////////////////////////
			if(!infl.empty()){
				tmp = infl.front();
				infl.pop();
				tmp_round = user[tmp].round_time;
				times_result_num[tmp_round]++;
			} else{
				tmp_round = put_time[next_seed];
				continue;
			}
			//printf("tmp_round=%d %d %d\n",tmp_round,next_seed,put_activenode[next_seed]);
			int friendnum = user[tmp].friends.size();
			for(int i = 0, j; i < friendnum; i++){
				j = user[tmp].friends[i].id;
				if(!user[j].isActived && take_random() < user[tmp].friends[i].closeness){
					infl.push(j);
					user[j].round_time = user[tmp].round_time + 1;
					user[j].isActived = 1;
				}
			}
		}//while(!infl.empty())
	}//for(simulateTimes)
	for(int i = 0; i < SeedsNumber * 20; i++){ //caulate the average of each round
		influenced_num_round[i] = times_result_num[i] / (double) SimulateTimes;
	}
}

int countEffectiveRound(double numberOfNodeBeEffectedForEachRound[]){
	bool startToCountEffectiveRound = false;
	int numOfEffectiveRound = 0;
	for(int k = 0; numberOfNodeBeEffectedForEachRound[k] > 0.0; k++){
		if(numberOfNodeBeEffectedForEachRound[k] > BasicInfluenceNumber)
			startToCountEffectiveRound = true;
		if(startToCountEffectiveRound){
			if(numberOfNodeBeEffectedForEachRound[k] > BasicInfluenceNumber)
				numOfEffectiveRound++;
			else break;
		}
	}
	return numOfEffectiveRound;
}

bool is_all_init_put(int seed[], int seednum){
	for(int i = 0; i < seednum; i++)
		if(user[seed[i]].isActived == 0)
			return false;
	return true;
}

void readFileAndBuildFrendRelation(FILE *edgeFile){
	maxId = 0;
	int lPerson, rPerson;
	double closeness;
	Friend inputfriend;
	while(fscanf(edgeFile, "%d %d %lf", &lPerson, &rPerson, &closeness) != EOF){
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

void outputResultToFile(FILE *outputFile, int seed[], int putTimeOfSeed[], int seedsNumber,
	double numberOfNodeBeEffectedForEachRound[], time_t startTime, time_t endTime){

	fprintf(outputFile, "The greedy run time is %ld\n", (endTime - startTime));
	fprintf(outputFile, "Greedy:\nThe seeds and num of his friends\n");
	for(int i = 0; i < seedsNumber; i++)
		fprintf(outputFile, "%4d\t%4lu\t%.3lf\n", seed[i], user[seed[i]].friends.size()
		, (double) user[seed[i]].expect);
	fprintf(outputFile, "Putting time of each seed\n");
	for(int i = 0; i < seedsNumber; i++)
		fprintf(outputFile, "%3d, ", putTimeOfSeed[i]);
	fprintf(outputFile, "\n");

	int numberOfEffectiveRound = 0;
	bool startToCountEffectiveRound = false;
	for(int k = 0; numberOfNodeBeEffectedForEachRound[k] > 0.0; k++){ //caulate the average of each round
		if(numberOfNodeBeEffectedForEachRound[k] > BasicInfluenceNumber)
			startToCountEffectiveRound = true;
		if(startToCountEffectiveRound){
			if(numberOfNodeBeEffectedForEachRound[k] > BasicInfluenceNumber)
				numberOfEffectiveRound++;
			else break;
		}
	}

	double numberOfNodeBeEffected = 0;
	for(int i = 0; numberOfNodeBeEffectedForEachRound[i] > 0.0; i++){ //caulate the average of each round
		numberOfNodeBeEffected += numberOfNodeBeEffectedForEachRound[i];
	}
	fprintf(outputFile, "%4d up:\t%3d\n", BasicInfluenceNumber, numberOfEffectiveRound);
	fprintf(outputFile, "the average num: \n%.3lf\n", numberOfNodeBeEffected);
	fprintf(outputFile, "average round\n");
	for(int i = 0; numberOfNodeBeEffectedForEachRound[i] > 1e-7; i++)
		fprintf(outputFile, "%.3lf\n", numberOfNodeBeEffectedForEachRound[i]);

}

double take_random(){
	double result = (double) rand() / (RAND_MAX + 1.0);
	return result;
}

void choiceSeedByRandom(int seed[]){
	double sum, tmp_influence[SeedsNumber * 20];
	int tmp_seed[5], tmp_puttime[5], tmp_effect[5];
	for(int i = 0, j; i < SeedsNumber; i++){ //choose seed
		int findTimes = 100;
		while(findTimes--){
			j = rand() % maxId;
			if(user[j].isActived == 1 || exist[j] == 0) continue;
			tmp_seed[0] = j;
			tmp_puttime[0] = 0;
			run_result(tmp_seed, tmp_puttime, tmp_influence, tmp_effect, 1);
			sum = 0;
			for(int k = 0; tmp_influence[k] > 0.0 && k < SeedsNumber * 20; k++){
				sum += tmp_influence[k];
			}
			if(sum > BasicAbilityOfrSeed)
				break;
		}
		if(findTimes == 0){
			printf("no proper seed\n");
		} else{
			printf("seed: %d id: %d effected num: %lf\n", i, j, sum);
		}
		seed[i] = j;
	}
}



