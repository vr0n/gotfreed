#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define LOG(f, m) fprintf(f, "%ld: %s\n", time(0), (m));

void code_cave() {
  printf("This function is also a code cave!\n");  
  printf("It executes some nonsense code just to give it a somewhat realistic size...\n");

  int* useless_int = malloc(sizeof(int));
  *useless_int = 12345;

  if (*useless_int == 12345) {
    printf("The number has been assigned...\n");
  } else {
    printf("The number has not been assigned...\n");
  }

  free(useless_int);
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
