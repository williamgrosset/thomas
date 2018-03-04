#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct Train {
  int number;
  // 0 = low; 1 = high
  int priority;
  int loading_time;
  int crossing_time;
} Train;

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

  while (EOF != fscanf(fp, "%c %d %d\n", &direction, &loading_time, &crossing_time)) {
    printf("%c %d %d\n", direction, loading_time, crossing_time);
    int priority = isupper(direction) ? 1 : 0;

    Train train;
    train.number = count++;
    train.priority = priority;
    train.loading_time = loading_time;
    train.crossing_time = crossing_time;
  }

  fclose(fp);
  return 0;
}
