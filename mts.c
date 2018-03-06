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

/****** /MUTEXES & CONDITION VARIABLES ******/

/****** TRAIN ******/

typedef struct Train {
  int id;
  int priority; // (0 = low; 1 = high)
  float loading_time; // (seconds)
  float crossing_time; // (seconds)
} Train;

void simulateWork(float time_duration) {
  usleep(time_duration);
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
  int count;
} ThreadParams;

/***** THREAD PARAMETERS ******/

/****** STATION ******/

bool isEmpty(int station_size) {
  return station_size == 0;
}

bool isFull(int station_size) {
  return station_size == MAX_SIZE;
}

struct Train peek(struct Train station[], int station_size) {
  return station[station_size - 1];
}

struct Train removeTrain(struct Train station[], int station_size) {
  return station[--station_size];
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

void* process_train(void *arg) {
  printf("Thread created.\n");
  ThreadParams *threadParams = (ThreadParams*) arg;
  Train train = threadParams->train;
  long count = threadParams->count;
  printf("%lu\n", count);

  if (count == 2) {
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

  printf("THREAD IS ALIVE!\n");
  // simulateWork(train.loading_time)

  // TODO: Lock station mutex, enqueue, release station mutex

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

  // Create PriorityQueue for West & East station
  int west_station_size = 0;
  int east_station_size = 0;
  struct Train WestStation[MAX_SIZE];
  struct Train EastStation[MAX_SIZE];

  int threads_count = 0;
  float loading_time;
  float crossing_time;
  char direction;
  TrainThread trainThreads[MAX_SIZE];

  while (EOF != fscanf(fp, "%c %f %f\n", &direction, &loading_time, &crossing_time)) {
    // printf("%c %f %f\n", direction, loading_time, crossing_time);
    pthread_t thread;
    struct TrainThread trainThread = {
      .thread = thread,
      .train = {
        .id = threads_count,
        .priority = isupper(direction) ? 1 : 0,
        .loading_time = loading_time / 10.0,
        .crossing_time = crossing_time / 10.0
      }
    };

    trainThreads[threads_count++] = trainThread;

    /* Temporary
    if (direction == 'w' || direction == 'W') {
      addTrain(WestStation, &west_station_size, trainThread.train);
    } else {
      addTrain(EastStation, &east_station_size, trainThread.train);
    }*/
  }

  // Temporary
  // displayStation(EastStation, east_station_size);

  long i;
  for (i = 0; i < threads_count; i++) {
    // TODO: Pass in Train from trainThreads and use count, num_threads
    struct ThreadParams *threadParams = (ThreadParams*) malloc(sizeof(ThreadParams));
    threadParams->train = trainThreads[i].train;
    threadParams->count = i;
    pthread_create(&trainThreads[i].thread, NULL, &process_train, (void *) threadParams);
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
  // while (n trains to be dispatched) {
    // pthread_mutex_lock(&track_lock);
    // pthread_cond_wait(&ready_train);
  // }

  // TODO: When to join pthreads together?
  long t;
  for (t = 0; t < threads_count; t++) {
    pthread_join(trainThreads[t].thread, NULL);
  }

  // TODO: Destroy mutexes
  fclose(fp);
  return 0;
}
