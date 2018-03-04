# thomas
:train: Thomas the multi-threaded train scheduler.

## TODO
+ Create PriorityQueue with specialized object
+ Use libraries for memory handling data structures (pls)
+ Create threads for each train
+ Begin loading once all threads are created
+ Update design document and add `changes.txt`
+ ...

## Overview
This project was an assignment for the [Operating Systems](https://github.com/williamgrosset/thomas/blob/master/csc360_p2.pdf) class at the University of Victoria. The multi-threaded train scheduler is a simulation of priority-based trains being loaded concurrently and dispatched one at a time across a main track. See the [design document](https://github.com/williamgrosset/thomas/blob/master/csc360_p2_solutions.pdf) for more details.

### Usage
#### Compile
```
make setup
```

#### Run train scheduler
```
make run
```

#### Clean build
```
make clean
```
