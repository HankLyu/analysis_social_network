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
const int BasicAbilityOfrSeed = 500;
const int BasicInfluenceNumber = 100;
const int MaxRound = 100;

struct Friend{
	int id;
	double closeness;
};

struct Social{
	int id;
	bool isActived; //if 1 this is be infected, if 0, otherwise
	int round_time;
	double expect;
	vector<Friend> friends;
};

struct Result{
	int seedsNumber;
	int seed[SeedsNumber];
	int putTimeOfSeed[SeedsNumber];
	int timeOfSeedBeEffectedBeforePut[SeedsNumber];
	double numberOfNodeBeEffectedForEachRound[MaxRound];
};

Social user[PeopleSize];
int exist[PeopleSize];
int maxId; //maxId to record the max id

double take_random();
void choiceSeedByRandom(int seed[]);
void greedy(int seed[], int putTimeOfSeed[], int seedsNumber,
	int timesOfSeedBeEffectedBeforePut[], double numberOfNodeBeEffectedForEachRound[]);
void readFileAndBuildFrendRelation(FILE *edgeFile);
void outputResultToFile(FILE *outputFile, const Result result, time_t runtimeOfAlgorithm);

int main(int argc, char* argv[]){
	int seed[SeedsNumber];
	double numberOfNodeBeEffectedForEachRound[SeedsNumber * 20] = {0};
	int timesOfSeedBeEffectedBeforePut[SeedsNumber] = {0};
	int putTimeOfSeed[SeedsNumber] = {0};
	FILE *edgeFile, *outputFile;
	time_t startTime, endTime;
	Result result;

	edgeFile = fopen(argv[1], "r");
	outputFile = fopen("greedy_put.txt", "w");
	memset(user, 0, sizeof(user));

	readFileAndBuildFrendRelation(edgeFile);
	srand(time(NULL));
	choiceSeedByRandom(seed);


	startTime = time(NULL);
	greedy(seed, putTimeOfSeed, SeedsNumber, timesOfSeedBeEffectedBeforePut, numberOfNodeBeEffectedForEachRound);
	endTime = time(NULL);

	result.seedsNumber = SeedsNumber;
	memcpy(result.numberOfNodeBeEffectedForEachRound,
		numberOfNodeBeEffectedForEachRound,
		sizeof(numberOfNodeBeEffectedForEachRound));
	memcpy(result.seed, seed, sizeof(seed));
	memcpy(result.putTimeOfSeed, putTimeOfSeed, sizeof(putTimeOfSeed));
	memcpy(result.timeOfSeedBeEffectedBeforePut,
		timesOfSeedBeEffectedBeforePut,
		sizeof(timesOfSeedBeEffectedBeforePut));
	outputResultToFile(outputFile, result, (endTime - startTime));

	fclose(edgeFile);
	fclose(outputFile);
	return 0;
}

void runWithFixSeedsAndTime(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum);
void findPutTimeForFixOrderSeeds(int seed[], int putTimeOfSeed[], double numberOfNodeBeEffectedForEachRound[],
	int seedBeEffectedBeforePut[], const int seedNum, const int startFindNum);
int countEffectiveRoundWithErr(const double numberOfNodeBeEffectedForEachRound[], const int Err);

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
			numOfEffectiveRoundOfSeed[testSeed] = countEffectiveRoundWithErr(numberOfNodeBeEffectedForEachRound, 0);

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
			printf("\ntest seed: %d ", seed[testSeed]);
			printf("peak: %d \n", peakOfTestSeed);
			printf("number of effective round: %d ", numberOfCurrentEffectiveRound);
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
		int numOfEffectiveRoundBestSeed = countEffectiveRoundWithErr(numberOfNodeBeEffectedForEachRound, 0);

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
		//printf("\nbegin_round is %d\n peak is : %.3lf\n", peakRound, newPeak);
		printf("%dth put seed is %d, put time is %d\n\n\n", nthSeed + 1, bestPutOrderForSeeds[nthSeed], putTimeOfSeed[nthSeed]);
		nthSeed++;
	}//while(nthSeed < seednum)
	for(int i = 0; i < seedsNumber; i++)
		seed[i] = bestPutOrderForSeeds[i];
	for(int i = 0; i < seedsNumber; i++){
		timesOfSeedBeEffectedBeforePut[i] = 0;
	}
	runWithFixSeedsAndTime(seed, putTimeOfSeed, numberOfNodeBeEffectedForEachRound, timesOfSeedBeEffectedBeforePut, seedsNumber);
}

void findPutTimeForFixOrderSeeds(int seed[], int putTimeOfSeed[], double numberOfNodeBeEffectedForEachRound[],
	int seedBeEffectedBeforePut[], const int seedNum, const int startFindNum){

	double bestResultForEachRound[SeedsNumber * 20];
	const int TestRoundRange = 15;

	for(int i = startFindNum; i < seedNum; i++){
		if(i == 0){
			putTimeOfSeed[0] = 0;
			//printf("choose first seed: %d\n", seed[0]);
			runWithFixSeedsAndTime(seed, putTimeOfSeed, numberOfNodeBeEffectedForEachRound, seedBeEffectedBeforePut, i + 1);
			continue;
		}
		int bestPutTime = putTimeOfSeed[i - 1];
		int maxEffectRound = 0, numEffectRoundForThisTime;
		for(int testPutTime = putTimeOfSeed[i - 1]; testPutTime < putTimeOfSeed[i - 1] + TestRoundRange; testPutTime++){
			putTimeOfSeed[i] = testPutTime;
			runWithFixSeedsAndTime(seed, putTimeOfSeed, numberOfNodeBeEffectedForEachRound, seedBeEffectedBeforePut, i + 1);
			numEffectRoundForThisTime = countEffectiveRoundWithErr(numberOfNodeBeEffectedForEachRound, 3);
			if(maxEffectRound < numEffectRoundForThisTime){
				maxEffectRound = numEffectRoundForThisTime;
				bestPutTime = testPutTime;
				memcpy(bestResultForEachRound, numberOfNodeBeEffectedForEachRound, sizeof(bestResultForEachRound));
			}
		}//for(int j=put_time[i]+1; j<put_time[i]+15;j++)
		putTimeOfSeed[i] = bestPutTime;
		printf("this seed %d: best_put_time is %d, round: %d\n", seed[i], putTimeOfSeed[i], maxEffectRound);
		memcpy(numberOfNodeBeEffectedForEachRound, bestResultForEachRound, sizeof(bestResultForEachRound)); //return the best of the influence of this seed
	}//for(int i=1; i<seednum; i++)
}

bool isAllSeedBePut(int seed[], int seednum);

void runWithFixSeedsAndTime(int seed[], int putTimeOfSeed[], double numberOfNodeBeEffectedForEachRound[], int seedBeEffectedBeforePut[], int seedNum){
	bool isSeedBePut[SeedsNumber] = {false};
	int nextSeed;
	int sumOfNodeForEachRound[SeedsNumber * 20] = {0};
	queue<int>infl; //to do bfs

	for(int t = 0; t < SimulateTimes; t++){
		while(!infl.empty()) infl.pop();
		for(int i = 0; i < maxId; i++){ //initial each node
			user[i].isActived = 0;
			user[i].round_time = -1;
		}
		nextSeed = 0;
		memset(isSeedBePut, false, sizeof(isSeedBePut));
		int currentNode, currentRound = 0;
		while(!infl.empty() || !isAllSeedBePut(seed, seedNum)){
			//for each round to put the seed once, and check this seed
			//has been influenced. if yes record this seed has been influenced, no put it into queue
			if(putTimeOfSeed[nextSeed] == currentRound &&
				nextSeed < seedNum &&
				isSeedBePut[nextSeed] == false){

				isSeedBePut[nextSeed] = true;
				if(user[seed[nextSeed]].isActived == 0){
					infl.push(seed[nextSeed]);
					user[seed[nextSeed]].isActived = 1;
					user[seed[nextSeed]].round_time = currentRound;
				} else if(nextSeed < seedNum){
					seedBeEffectedBeforePut[nextSeed]++;
				}
				nextSeed++;
			}
			if(!infl.empty()){
				currentNode = infl.front();
				infl.pop();
				currentRound = user[currentNode].round_time;
				sumOfNodeForEachRound[currentRound]++;
			} else{
				currentRound = putTimeOfSeed[nextSeed];
				continue;
			}
			//printf("tmp_round=%d %d %d\n",tmp_round,next_seed,put_activenode[next_seed]);
			int friendnum = user[currentNode].friends.size();
			for(int i = 0, j; i < friendnum; i++){
				j = user[currentNode].friends[i].id;
				if(!user[j].isActived && take_random() < user[currentNode].friends[i].closeness){
					infl.push(j);
					user[j].round_time = user[currentNode].round_time + 1;
					user[j].isActived = 1;
				}
			}
		}//while(!infl.empty())
	}//for(simulateTimes)
	for(int i = 0; i < SeedsNumber * 20; i++){ //caulate the average of each round
		numberOfNodeBeEffectedForEachRound[i] = sumOfNodeForEachRound[i] / (double) SimulateTimes;
	}
}

int countEffectiveRoundWithErr(const double NumberOfNodeBeEffectedForEachRound[], const int Err){
	bool startToCountEffectiveRound = false;
	int numOfEffectiveRound = 0;
	for(int k = 0; NumberOfNodeBeEffectedForEachRound[k] > 0.0; k++){
		if(NumberOfNodeBeEffectedForEachRound[k] > BasicInfluenceNumber)
			startToCountEffectiveRound = true;
		if(startToCountEffectiveRound){
			if(NumberOfNodeBeEffectedForEachRound[k] > BasicInfluenceNumber + Err)
				numOfEffectiveRound++;
			else break;
		}
	}
	return numOfEffectiveRound;
}

bool isAllSeedBePut(int seed[], int seednum){
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
}

void outputResultToFile(FILE *outputFile, const Result result, time_t runtimeOfAlgorithm){
	fprintf(outputFile, "The greedy run time is %ld\n", runtimeOfAlgorithm);
	fprintf(outputFile, "Greedy:\n"
		"seeds\n");
	for(int i = 0; i < result.seedsNumber; i++)
		fprintf(outputFile, "%4d\t%4lu\t%.3lf\n", result.seed[i], user[result.seed[i]].friends.size()
		, (double) user[result.seed[i]].expect);
	fprintf(outputFile, "Putting time of each seed\n");
	for(int i = 0; i < result.seedsNumber; i++)
		fprintf(outputFile, "%3d, ", result.putTimeOfSeed[i]);
	fprintf(outputFile, "\n");
	int numberOfEffectiveRound = countEffectiveRoundWithErr(result.numberOfNodeBeEffectedForEachRound, 0);

	double totalNumberOfNodeBeEffected = 0;
	for(int i = 0; result.numberOfNodeBeEffectedForEachRound[i] > 0.0; i++){ //caulate the average of each round
		totalNumberOfNodeBeEffected += result.numberOfNodeBeEffectedForEachRound[i];
	}
	fprintf(outputFile, "%4d up:\t%3d\n", BasicInfluenceNumber, numberOfEffectiveRound);
	fprintf(outputFile, "the total number of effected nodes is: \n%.3lf\n", totalNumberOfNodeBeEffected);
	fprintf(outputFile, "the number of effected nodes of each round\n");
	for(int i = 0; result.numberOfNodeBeEffectedForEachRound[i] > 1e-7; i++)
		fprintf(outputFile, "round %d:\t%.3lf\n", i + 1, result.numberOfNodeBeEffectedForEachRound[i]);
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
			runWithFixSeedsAndTime(tmp_seed, tmp_puttime, tmp_influence, tmp_effect, 1);
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



