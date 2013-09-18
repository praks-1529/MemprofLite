#include <sys/types.h>
#include <stdio.h>
void* malloc(size_t a_size) {
  printf("Do not come (static)\n");
}
