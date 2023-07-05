#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define LOG(f, m) fprintf(f, "%ld: %s\n", time(0), (m));

void false_func() {
  FILE *log;
  char* msg = "Injected log";
  printf("Writing injected log...\n");
  log = fopen("./log.file", "a");
  LOG(log, msg);
  fclose(log);

  exit(123);
}

int main(int argc, char* argv[]) {
  int i = 0;
  FILE *log;
  char* msg = "Regular log";
  while(1) {
    printf("Writing to log...\n");
    log = fopen("./log.file", "a");
    LOG(log, msg);
    fclose(log);
    printf("Sleeping for a while.");
    for (i = 0; i < 30; i++) {
      printf(".");
      fflush(stdout);
      sleep(1);
    }
    printf("\n");
  }
  return 0;
}
