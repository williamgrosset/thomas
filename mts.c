#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#define MAX_SIZE 1024

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

int main() {
  FILE *fp;
  
  // TODO: Pass in arg for file
  fp = fopen("trains.txt", "r");
  if (fp == NULL) {
    perror("Error opening file.");
    return -1;
  }

  char direction;
  float loading_time;
  float crossing_time;
  int count = 0;

  // Create PriorityQueue for east_station and west_station (two instances of Station)
  int west_station_size = 0;
  int east_station_size = 0;
  struct Train WestStation[MAX_SIZE];
  struct Train EastStation[MAX_SIZE];

  // TODO: Detect how many trains (lines) in file
  while (EOF != fscanf(fp, "%c %f %f\n", &direction, &loading_time, &crossing_time)) {
    printf("%c %f %f\n", direction, loading_time, crossing_time);
    Train train;
    train.id = count++;
    train.priority = isupper(direction) ? 1 : 0;
    train.loading_time = loading_time / 10.0;
    train.crossing_time = crossing_time / 10.0;

    // TODO:
    // Create threads for each train
    // Train threads LOCK TRACK MUTEX and wait to be signaled to begin loading
    // Main thread waits for last train thread to signal main (all threads created)
    // Main thread is signaled and broadcasts to begin loading and RELEASES TRACK MUTEX
    // Train threads begin loading and begin STATION ENQUEUE PROCESS

    // Temporary
    if (direction == 'w' || direction == 'W') {
      addTrain(WestStation, &west_station_size, train);
    } else {
      addTrain(EastStation, &east_station_size, train);
    }
  }

  // Temporary
  displayStation(EastStation, east_station_size);

  fclose(fp);
  return 0;
}
