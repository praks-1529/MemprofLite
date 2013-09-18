#include <sys/types.h>
#include <stdio.h>
void* malloc(size_t a_size) {
  printf("Do not come (static)\n");
}

void free(void* a_ptr) {
  printf("Free:Do not come (static)\n");
}
