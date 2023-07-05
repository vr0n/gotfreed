#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

struct a_struct {
  int id;
  char msg[32];
};

void test_func() {
  printf("We are in the test_func now\n");

  return;
}

int main(int argc, char** argv) {
  printf("We are going to create a structure now...\n");

  struct a_struct* str_mem = malloc(sizeof(struct a_struct));

  str_mem->id = 1;
  strcpy(str_mem->msg, "Test message");

  printf("Your struct is now on the heap...\n");
  printf("Here are its details...\n");
  printf("\nID:  %d\n", str_mem->id);
  printf("MSG: %s\n", str_mem->msg);

  free(str_mem);

  return 0;
}
