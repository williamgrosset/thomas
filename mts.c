#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#define MAX_SIZE 1024
#define BILLION 1E9

// TEMPORARY GLOBAL VARIABLE(S)
bool can_load_bool = false;
bool threads_created_bool = false;

/****** MUTEXES & CONDITION VARIABLES ******/

pthread_mutex_t track_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t west_station_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t east_station_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t can_load = PTHREAD_COND_INITIALIZER;
pthread_cond_t threads_created = PTHREAD_COND_INITIALIZER;
pthread_cond_t station_ready = PTHREAD_COND_INITIALIZER;

/****** /MUTEXES & CONDITION VARIABLES ******/

/****** TRAIN ******/

typedef struct Train {
  int id;
  int priority; // (0 = low; 1 = high)
  float loading_time; // (seconds)
  float crossing_time; // (seconds)
  char direction;
  pthread_cond_t can_cross;
} Train;

void simulateWork(int duration) {
  usleep(duration);
}

/****** /TRAIN ******/

/****** TRAIN THREAD ******/

typedef struct TrainThread {
  struct Train train;
  pthread_t thread;
} TrainThread;

/****** /TRAIN THREAD ******/

/***** THREAD PARAMETERS ******/

typedef struct ThreadParams {
  struct Train train;
  int curr_count;
  int thread_count;
} ThreadParams;

/***** THREAD PARAMETERS ******/

/****** STATION ******/

// Initialize PriorityQueues for West & East station
int west_station_size = 0;
int east_station_size = 0;
struct Train WestStation[MAX_SIZE];
struct Train EastStation[MAX_SIZE];

bool isEmpty(int station_size) {
  return station_size == 0;
}

bool isFull(int station_size) {
  return station_size == MAX_SIZE;
}

struct Train peek(struct Train station[], int station_size) {
  return station[station_size - 1];
}

struct Train dequeue(struct Train station[], int *station_size) {
  return station[--*station_size];
}

void enqueue(struct Train station[], int *station_size, pthread_mutex_t *station_lock, struct Train train) {
  pthread_mutex_lock(station_lock);
  int i = 0;

  if (!isFull(*station_size)) {
    if (*station_size == 0) {
      station[*station_size] = train;
    } else {
      for (i = *station_size - 1; i >= 0; i--) {
        if (train.priority <= station[i].priority && train.id > station[i].id) {
          station[i + 1] = station[i];
        } else {
          break;
        }
      }

      // Insert train
      station[i + 1] = train;
    }
    *station_size += 1;
  }
  pthread_mutex_unlock(station_lock);
}

/****** FOR TESTING ******/
void displayStation(struct Train station[], int station_size) {
  int i = 0;

  for (i = 0; i < station_size; i++) {
    Train train = station[i];
    printf("Index: %i, ID: %i, Priority: %i, Loading: %f, Crossing: %f\n", i, train.id, train.priority,
        train.loading_time, train.crossing_time);
  }
}

/****** /STATION ******/

void calc_accum_time(struct timespec start, struct timespec stop) {
  if (clock_gettime(CLOCK_REALTIME, &stop) == -1) {
    perror("Could not get time.");
    exit(EXIT_FAILURE);
  }

  double accum = (stop.tv_sec - start.tv_sec) + (stop.tv_nsec - start.tv_nsec) / BILLION;
  printf("%02d:%02d:%04.1f ", (int) accum / (60 * 60), (int) accum / 60, accum);
}

void* processTrain(void *arg) {
  ThreadParams *threadParams = (ThreadParams*) arg;
  Train train = threadParams->train;
  struct timespec start, stop;

  if (threadParams->curr_count == (threadParams->thread_count - 1)) {
    // Signal that the last train has been created
    pthread_mutex_lock(&track_lock);
    pthread_cond_signal(&threads_created);
    threads_created_bool = true;
    pthread_mutex_unlock(&track_lock);
  }

  // Wait to begin loading
  pthread_mutex_lock(&track_lock);
  while (!can_load_bool) pthread_cond_wait(&can_load, &track_lock);
  pthread_mutex_unlock(&track_lock);

  if (clock_gettime(CLOCK_REALTIME, &start) == -1) {
    perror("Could not get time.");
    exit(EXIT_FAILURE);
  }

  simulateWork(train.loading_time * 1000000);
  calc_accum_time(start, stop);
  printf("Train %i is ready to go %c\n", train.id, train.direction);

  // Lock station mutex, enqueue, release station mutex
  if (train.direction == 'w' || train.direction == 'W') {
    enqueue(WestStation, &west_station_size, &west_station_lock, train);
  } else {
    enqueue(EastStation, &east_station_size, &east_station_lock, train);
  }

  // Signal main thread
  pthread_mutex_lock(&track_lock);
  pthread_cond_signal(&station_ready);
  pthread_mutex_unlock(&track_lock);

  // pthread_mutex_lock(&track_lock);
  // while () pthread_cond_wait(&train.can_cross, &track_lock);
  // simulateWork(train.crossing_time);
  // pthread_mutex_unlock(&track_lock);

  // free(arg);
  pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
  FILE *fp;
  
  fp = fopen(argv[1], "r");
  if (fp == NULL) {
    perror("Error opening file.");
    return(EXIT_FAILURE);
  }

  // Lock track mutex initially
  pthread_mutex_lock(&track_lock);

  int thread_count = 0;
  float loading_time;
  float crossing_time;
  char direction;
  TrainThread trainThreads[MAX_SIZE];

  // Scan input file and initialize train threads
  while (EOF != fscanf(fp, "%c %f %f\n", &direction, &loading_time, &crossing_time)) {
    printf("%c %f %f\n", direction, loading_time, crossing_time);
    pthread_t thread;
    struct TrainThread trainThread = {
      .thread = thread,
      .train = {
        .id = thread_count,
        .priority = isupper(direction) ? 1 : 0,
        .loading_time = loading_time / 10.0,
        .crossing_time = crossing_time / 10.0,
        .direction = direction,
        .can_cross = PTHREAD_COND_INITIALIZER
      }
    };

    trainThreads[thread_count++] = trainThread;
  }

  // Temporary
  // displayStation(EastStation, east_station_size);

  // Create threads
  for (int i = 0; i < thread_count; i++) {
    struct ThreadParams *threadParams = (ThreadParams*) malloc(sizeof(ThreadParams));
    threadParams->train = trainThreads[i].train;
    threadParams->curr_count = i;
    threadParams->thread_count = thread_count;
    pthread_create(&trainThreads[i].thread, NULL, &processTrain, (void *) threadParams);
  }

  // Wait until last train thread has been created
  while(!threads_created_bool) pthread_cond_wait(&threads_created, &track_lock);
  pthread_mutex_unlock(&track_lock);

  // Broadcast to all trains to begin loading
  pthread_mutex_lock(&track_lock);
  pthread_cond_broadcast(&can_load);
  can_load_bool = true;
  pthread_mutex_unlock(&track_lock);

  // Wait to dispatch a train across main track
  int trains_dispatched = 0;
  while (trains_dispatched < thread_count) {
    pthread_mutex_lock(&track_lock);
    while (isEmpty(west_station_size) && isEmpty(east_station_size)) pthread_cond_wait(&station_ready, &track_lock);
    // Begin dispatch algorithm
    if (!isEmpty(west_station_size) && !isEmpty(east_station_size)) {
      dequeue(WestStation, &west_station_size);
      // choose which one to remove
    } else if (!isEmpty(west_station_size)) {
      dequeue(WestStation, &west_station_size);
    } else {
      dequeue(EastStation, &east_station_size);
    }
    // Signal appropriate train and release mutex
    printf("Dispatched train across track and removed from Q.\n");
    pthread_mutex_unlock(&track_lock);

    // Wait train to be signaled by train (done crossing)
    trains_dispatched++;
  }

  // TODO: When to join pthreads together?
  for (long t = 0; t < thread_count; t++) {
    pthread_join(trainThreads[t].thread, NULL);
  }

  // TODO: Destroy mutexes
  fclose(fp);
  return(EXIT_SUCCESS);
}
