# thomas
:train: Thomas the multi-threaded train scheduler.

## Overview
This project was an assignment for the [Operating Systems](https://github.com/williamgrosset/thomas/blob/master/csc360_p2.pdf) class at the University of Victoria. The multi-threaded train scheduler is a simulation of trains being scheduled and dispatched across a main track. Each train is represented as a thread and is in charge of it's loading and crossing time, which is simualted using `usleep()`. The main thread is represented as the dispatcher thread and waits for a signal to dispatch a train across the main track. See the [design document](https://github.com/williamgrosset/thomas/blob/master/csc360_p2_solutions.pdf) for more details. 

### Updated Design Document
+ 5 mutexes
+ n + 2 convars (`canLoad` and `isReady` is global)

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
