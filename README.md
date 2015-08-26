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

## forExcel.py for draw picture in Excel

Install requirements.txt

    pip install requirements.txt

Draw picture

    python forExcel.py
