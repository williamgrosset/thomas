#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <pthread.h>
#include <unistd.h>
#define MAX_SIZE 1024

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

/****** /TRAIN ******/

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

      // insert train
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

void simulate_train_work(float time_duration) {
  usleep(time_duration);
}

void* process_train(void *arg) {
  printf("Thread created.\n");
  long count = (int) arg;
  printf("%lu\n", count);

  if (count == 2) {
    // Signal that all thats have been created 
    printf("Last thread created.\n");
    pthread_mutex_lock(&track_lock);
    pthread_cond_signal(&threads_created);
    pthread_mutex_unlock(&track_lock);
  }

  pthread_mutex_lock(&track_lock);
  // Wait to begin loading all trains at same time
  pthread_cond_wait(&can_load, &track_lock);
  pthread_mutex_unlock(&track_lock);
  printf("THREAD IS ALIVE!\n");
  // simulate_train_work(train.loading_time)

  // TODO: Lock station mutex, enqueue, release station mutex
  return NULL;
}

int main(int argc, char* argv[]) {
  FILE *fp;
  
  // TODO: Pass in arg for file
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

  int num_threads = 0;
  float loading_time;
  float crossing_time;
  char direction;
  pthread_t threads[MAX_SIZE];

  while (EOF != fscanf(fp, "%c %f %f\n", &direction, &loading_time, &crossing_time)) {
    // printf("%c %f %f\n", direction, loading_time, crossing_time);
    pthread_t thread;
    Train train;

    train.id = num_threads;
    train.priority = isupper(direction) ? 1 : 0;
    train.loading_time = loading_time / 10.0;
    train.crossing_time = crossing_time / 10.0;

    threads[num_threads++] = thread;

    /* Temporary
    if (direction == 'w' || direction == 'W') {
      addTrain(WestStation, &west_station_size, train);
    } else {
      addTrain(EastStation, &east_station_size, train);
    }*/
  }

  long i;
  for (i = 0; i < num_threads; i++) {
    // TODO: pass in i and num_threads (and more args)
    pthread_create(&threads[i], NULL, &process_train, (void *) i);
  }

  // Wait until all threads have been created (receive signal from TT)
  pthread_cond_wait(&threads_created, &track_lock);
  pthread_mutex_unlock(&track_lock);

  // Broadcast to all trains that are waiting to begin loading
  pthread_mutex_lock(&track_lock);
  pthread_cond_broadcast(&can_load);
  pthread_mutex_unlock(&track_lock);

  // while (n trains to be dispatched) {
  // Wait to dispatch a train across main track
  // pthread_mutex_lock(&track_lock);
  // pthread_cond_wait(&ready_train);

  long t;
  for (t = 0; t < num_threads; t++) {
    pthread_join(threads[t], NULL);
  }

  // Temporary
  // displayStation(EastStation, east_station_size);

  fclose(fp);
  return 0;
}
