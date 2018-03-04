#include <stdio.h>
#include <stdlib.h>

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

  while (EOF != fscanf(fp, "%c %d %d\n", &direction, &loading_time, &crossing_time)) {
    printf("%c %d %d\n", direction, loading_time, crossing_time);
  }

  fclose(fp);
  return 0;
}
