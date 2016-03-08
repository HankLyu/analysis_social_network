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
const int basicAbilityOfrSeed = 100;
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

class SocialMan {
public:

	SocialMan(int Id) {
		id = Id;
		exist = 0;
		activeStatus = 0;
		beActivedTime = -1;
		influenceAllHisFriendAbility = 0;
		hisFriends.clear();
	}

	bool beActivedAt(int roundTime) {
		activeStatus = 1;
		beActivedTime = roundTime;
		return true;
	}

	bool isActived()const {
		if (activeStatus == 1)
			return true;
		else
			return false;
	}

	bool isExist()const {
		return exist;
	}

	bool makeFriends(int idName, double theirCloseness) {
		exist = true;
		Friends newFriend;
		newFriend.id = idName;
		newFriend.closeness = theirCloseness;
		hisFriends.push_back(newFriend);
		return true;
	}

	bool cleanStatus() {
		activeStatus = 0;
		beActivedTime = -1;
	}

	const vector<int> friendsIsInfluenceByMe() {
		vector<int> resultFriends;
		int friendLength = hisFriends.size();
		double interactiveThisTime;
		for (int i = 0; i < friendLength; i++) {
			interactiveThisTime = (double) rand() / (RAND_MAX + 1.0);
			if (interactiveThisTime < hisFriends[i].closeness)
				resultFriends.push_back(hisFriends[i].id);
		}
		return resultFriends;
	}
private:
	int id;
	bool exist;
	bool activeStatus;
	int beActivedTime;
	double influenceAllHisFriendAbility;

	struct Friends {
		int id;
		double closeness;
	};
	vector<Friends> hisFriends;
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
void greedy(int seed[], int put_time[], double influenced_num_round[], int seed_be_effected[], int seednum);
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
	int seed_be_effected[seedsNumber] = {0}; //count it is active node but it have been effected times
	int put_time[seedsNumber] = {0}; //the round should be put

	start_time = time(NULL);
	greedy(seed, put_time, influenced_num_round, seed_be_effected, seedsNumber);
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

void greedy(int seed[], int putTimeOfSeed[], double influenced_num_round[],
	int seedBeEffectedBeforePut[], int numOfSeed) {
	int nthSeed = 0, be_put[seedsNumber] = {0};
	int last_effect = 0, seed_effect[seedsNumber] = {0};
	double best_influenced_num_round[seedsNumber * 20];
	//last_effect is to record how many rounds are over thres
	//seed_effect is to record how many rounds are over thres to check whether this seed have enough influence
	int best_put_order[seedsNumber], begin_round = 0, tmp_begin_round;
	//begin_round is to record the round of peak.
	int best_put_time; //record the time of the current seed
	for (int i = 0; i < nthSeed; i++) { //if the order of the part seeds is sure, change the "nthSeed" valure 
		be_put[i] = 1;
		best_put_order[i] = seed[i];
	}
	while (nthSeed < numOfSeed) {
		bool have_influence = false;
		int min_overthres = 1e5; //to find the  best seed having the min peak for this order to make sure the wave is not too high
		int peak; //to find the peak of each seed
		int bestseed_thisround = -1; //to record which seed is best in this round

		memset(seed_effect, 0, sizeof (seed_effect));
		for (int i = 0; i < numOfSeed; i++) { //to 
			peak = 0;
			if (be_put[i] == 1) continue;

			best_put_order[nthSeed] = seed[i];
			run_puttime(best_put_order, putTimeOfSeed, influenced_num_round, seedBeEffectedBeforePut, nthSeed + 1, nthSeed);
			tmp_begin_round = begin_round;
			//chose the peak
			for (int j = tmp_begin_round; j < tmp_begin_round + 35; j++) { //check the influencu wave
				peak = max(peak, (int) influenced_num_round[j]);
			}
			bool upto_thres = false;
			for (int k = 0; influenced_num_round[k] > 0.0; k++) { //caulate the average of each round
				if (influenced_num_round[k] > basicInfluenceNumber) upto_thres = true;
				if (upto_thres) {
					if (influenced_num_round[k] > basicInfluenceNumber) seed_effect[i]++;
					else break;
				}
				//printf("%d: %.1lf\n", i, influenced_num_round[k]);
			}
			//printf("seed %d max over %d\n", seed[i], peak);
			if (peak < min_overthres && peak > basicInfluenceNumber
				&& seed_effect[i] > last_effect + 1) { //check whether the current seed is good
				min_overthres = peak; //to sure the new seed just over the threshold a bit
				bestseed_thisround = i;
				best_put_time = putTimeOfSeed[nthSeed];
				have_influence = true;
				memcpy(best_influenced_num_round, influenced_num_round, sizeof (best_influenced_num_round));
			}
			printf("the current best seed is: %d  peak: %d last_effect: %d, this seed effected %d\n"
				, seed[bestseed_thisround], peak, last_effect, seed_effect[i]);
		}//for(int i=0; i<seednum; i++)
		if (bestseed_thisround == -1) {
			printf("no proper seed!!!!!\n");
			for (int i = 0; i < seedsNumber; i++)
				if (be_put[i] == 0) {
					be_put[i] = 1;
					bestseed_thisround = i;
					best_put_time = putTimeOfSeed[nthSeed - 1];
					break;
				}
			have_influence = false;
		}
		double new_peak = 0;
		for (int j = tmp_begin_round; j < tmp_begin_round + 35; j++) {
			//printf("%lf\n",influenced_num_round[j]);
			if (new_peak < best_influenced_num_round[j]) {
				new_peak = best_influenced_num_round[j];
				begin_round = j;
			}
		}
		printf("begin_round is %d\n peak is : %.3lf", begin_round, new_peak);
		if (have_influence = true)
			last_effect = seed_effect[bestseed_thisround];
		else {
			best_put_time = putTimeOfSeed[nthSeed - 1];
			begin_round = best_put_time;
		}
		best_put_order[nthSeed] = seed[bestseed_thisround];
		be_put[bestseed_thisround] = 1;
		putTimeOfSeed[nthSeed] = best_put_time;
		printf("----------%d round seed is %d, put time is %d\n", nthSeed, best_put_order[nthSeed], putTimeOfSeed[nthSeed]);
		nthSeed++;
	}//while(nthSeed < seednum)
	for (int i = 0; i < numOfSeed; i++)
		seed[i] = best_put_order[i];
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




