# thomas
:train: A muiltithreaded train schedular simulation.

## TODO
+ Update design document and add `changes.txt`
+ Begin loading once all threads are created (signal MT once N trains have loaded)
+ MT locks track mutex
+ MT Create threads for each train and join
+ MT waits on signal to begin loading trains
+ MT broadcasts to all trains to begin loading
+ TT usleeps for loading time
+ TT adds train to PriorityQueue with mutex
+ Destroy mutex at end (audit strategy)
+ Clean-up nits and repo, error handling
+ ...
+ Other: Use libraries for memory handling data structures (pls)

*gif here*

## Overview
This project was an assignment for the [Operating Systems](https://github.com/williamgrosset/thomas/blob/master/csc360_p2.pdf) class at the University of Victoria. The multithreaded train scheduler is a simulation of priority-based trains being loaded concurrently and dispatched across a single track. See the [requirements document](https://github.com/williamgrosset/thomas/blob/master/csc360_p2.pdf) and [design document](https://github.com/williamgrosset/thomas/blob/master/csc360_p2_solutions.pdf) for more details.

### Usage
#### Compile
```
make setup
```

#### Run scheduler
```
make run f=trains.txt
# OR
./mts.o trains.txt
```

#### Run scheduler infinitely (testing)
```
bash -c 'while [ 0 ]; do make run f=trains.txt;done'
```

#### Clean build
```
make clean
```
