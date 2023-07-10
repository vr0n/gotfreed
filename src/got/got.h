#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <elf.h>
#include <sys/stat.h>

typedef struct got_entry {
  Elf64_Addr* addr;
  Elf64_Addr* val;
  char* symbol;
} got_entry;

typedef struct got_table {
  got_entry** entries;
  int size;
  int count;
} got_table;

void overwrite_got_entry(got_table* table, int overwrite, unsigned long* code_cave, int* fd_mem);

void read_got_table(got_table* table);

void populate_got_table(got_table* table, Elf64_Addr* got, int* fd_mem);

void generate_got_table(got_table* table);

Elf64_Addr get_got(unsigned char* elf, unsigned long* addr);

Elf64_Addr parse_elf(int* pid, unsigned long* addr, int* fd_mem);

char* get_base_addr(int* pid);

void write_to_cave(unsigned long* code_cave, char* shell_code, int* size, int* fd_mem);
