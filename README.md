#Analysis social network

## main.cpp for analysis network

main.cpp break.cpp upperbound.cpp lat_node_infl.cpp 
These four file are repersent four different method to decide the putting seeds and time.

##Execution:

###main:
g++ -o a main.cpp

./a edge.txt


output:greedy_put.txt

###break:
g++ -o a break.cpp

./a edge.txt


output:break_put.txt

###upperbound
g++ -o a upperbound.cpp

./a edge.txt


output:upperbound_put.txt

###last_node_infl
g++ -o a last_node_infl.cpp

./a edge.txt


output:last_put.txt

##edge file format

start_node end_node edge rate

such as:

0 1 0.1

0 2 0.01

1 0 0.01

...

##Other File

###confirm.cpp
when we run the four method, we usually run 1000 times. Then we use confirm.cpp to run the answer by these methods by 10000 times.

g++ -o a confirm.cpp

./a edge.txt

output:confirm_put.txt

###random_method.cpp
This is random putting method. The order of seed are random.

g++ -o a random_method.cpp

./a edge.txt

output:random_put.txt

###run_all.cpp
This is to run all node as a seed. To see which node has the best influence.

g++ -o a run_all.cpp

./a edge.txt

output: run_all.txt

###user.cpp
This is to know the information of each node, such as friends

g++ -o a user.cpp

./a edge.txt outputfile

output: outputfile

##Some Variable in cpp mean:
maxx ->It is the size of struct array used to save our node.

runtimes -> We use it to simulate that the node influences in network.

initnum -> It mean the number of seeds.

random_choice_thres -> We use it as a standard to choice some seeds which have some power of influence by random choose.

round_max -> It is the size of the array used to record the number of influenced nodes for each round.

thres1 -> The thresholds used to check whether these seeds influence one time of these round with the power.

thres2 -> It is not used now.

err -> To make sure the influence ability with no lucky random.

## forExcel.py for draw picture in Excel

Install requirements.txt

    pip install requirements.txt

Draw picture

    python forExcel.py
