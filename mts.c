#include <stdio.h>
#include <stdlib.h>

int main() {
  FILE *fp;
  char str[1024];
  
  // TODO: Pass in arg for file
  fp = open("trains.txt", "r");
  if (fp == NULL) {
    perror("Error opening file.");
    return -1;
  }
}
