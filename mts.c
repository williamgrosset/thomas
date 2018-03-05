#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#define MAX_SIZE 1024

/****** TRAIN ******/

typedef struct Train {
  int id;
  int priority; // (0 = low; 1 = high)
  int loading_time; // (seconds)
  int crossing_time; // (seconds)
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

void addTrain(struct Train station[], int station_size, struct Train train) {
  int i = 0;

  if (!isFull(station_size)) {
    if (station_size == 0) {
      station[station_size++] = train;
    } else {
      for (i = station_size - 1; i >= 0; i--) {
        if (train.priority < station[i].priority) {
          station[i + 1] = station[i];
        } else {
          break;
        }
      }

      // insert train
      station[i + 1] = train;
      station_size++;
    }
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
  int loading_time;
  int crossing_time;
  int count = 0;

  // Create PriorityQueue for east_station and west_station (two instances of Station)
  int west_station_size = 0;
  int east_station_size = 0;
  struct Train WestStation[MAX_SIZE];
  struct Train EastStation[MAX_SIZE];

  while (EOF != fscanf(fp, "%c %d %d\n", &direction, &loading_time, &crossing_time)) {
    printf("%c %d %d\n", direction, loading_time, crossing_time);
    Train train;
    train.id = count++;
    train.priority = isupper(direction) ? 1 : 0;
    train.loading_time = loading_time;
    train.crossing_time = crossing_time;

    if (direction == 'w' || direction == 'W') {
      addTrain(WestStation, east_station_size, train);
    } else {
      addTrain(EastStation, east_station_size, train);
    }
  }

  fclose(fp);
  return 0;
}
