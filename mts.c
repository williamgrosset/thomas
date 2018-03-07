#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#define MAX_SIZE 1024

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
} Train;

void simulateWork(float duration) {
  usleep(duration * 100);
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

struct Train removeTrain(struct Train station[], int *station_size) {
  return station[--*station_size];
}

void addTrain(struct Train station[], int *station_size, struct Train train) {
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

void* processTrain(void *arg) {
  printf("Thread created.\n");
  ThreadParams *threadParams = (ThreadParams*) arg;
  Train train = threadParams->train;

  if (threadParams->curr_count == (threadParams->thread_count - 1)) {
    // Signal that the last train has been created
    printf("Last thread created.\n");
    pthread_mutex_lock(&track_lock);
    pthread_cond_signal(&threads_created);
    threads_created_bool = true;
    pthread_mutex_unlock(&track_lock);
  }

  // Wait to begin loading
  pthread_mutex_lock(&track_lock);
  while (!can_load_bool) pthread_cond_wait(&can_load, &track_lock);
  pthread_mutex_unlock(&track_lock);

  printf("THREAD %i IS ALIVE!\n", threadParams->curr_count);
  simulateWork(train.loading_time);
  printf("Train %i is ready to go %c\n", train.id, train.direction);

  // Lock station mutex, enqueue, release station mutex
  if (train.direction == 'w' || train.direction == 'W') {
    pthread_mutex_lock(&west_station_lock);
    addTrain(WestStation, &west_station_size, train);
    pthread_mutex_unlock(&west_station_lock);
  } else {
    pthread_mutex_lock(&east_station_lock);
    addTrain(EastStation, &east_station_size, train);
    pthread_mutex_unlock(&east_station_lock);
  }

  // Signal main thread
  pthread_mutex_lock(&track_lock);
  printf("Thread %i sending signal to dispatcher.\n", train.id);
  pthread_cond_signal(&station_ready);
  pthread_mutex_unlock(&track_lock);

  // pthread_mutex_lock(&track_lock);
  // while () pthread_cond_wait(&can_cross, &track_lock);
  // pthread_mutex_unlock(&track_lock);

  // free(arg);
  pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
  FILE *fp;
  
  fp = fopen(argv[1], "r");
  if (fp == NULL) {
    perror("Error opening file.");
    return -1;
  }

  // Lock track mutex initially
  pthread_mutex_lock(&track_lock);

  // Initialize PriorityQueues for West & East station
  // see global

  int thread_count = 0;
  float loading_time;
  float crossing_time;
  char direction;
  TrainThread trainThreads[MAX_SIZE];

  // Scan input file and initialize train threads
  while (EOF != fscanf(fp, "%c %f %f\n", &direction, &loading_time, &crossing_time)) {
    // printf("%c %f %f\n", direction, loading_time, crossing_time);
    pthread_t thread;
    struct TrainThread trainThread = {
      .thread = thread,
      .train = {
        .id = thread_count,
        .priority = isupper(direction) ? 1 : 0,
        .loading_time = loading_time / 10.0,
        .crossing_time = crossing_time / 10.0,
        .direction = direction
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
      removeTrain(WestStation, &west_station_size);
      // choose which one to remove
    } else if (!isEmpty(west_station_size)) {
      removeTrain(WestStation, &west_station_size);
    } else {
      removeTrain(EastStation, &east_station_size);
    }
    //
    printf("Dispatched train across track and removed from Q.\n");
    pthread_mutex_unlock(&track_lock);
    trains_dispatched++;
  }

  // TODO: When to join pthreads together?
  for (long t = 0; t < thread_count; t++) {
    pthread_join(trainThreads[t].thread, NULL);
  }

  // TODO: Destroy mutexes
  fclose(fp);
  return 0;
}
