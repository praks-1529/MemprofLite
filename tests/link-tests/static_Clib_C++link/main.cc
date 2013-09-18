#include <pthread.h>
#include <stdlib.h>
//#include <memproflite.h>

void* abc(void* arg) {
  int i = 1;
  while(i--) {
    const int num_of_mallocs = 1;
    int *x[num_of_mallocs];
    int i;
    for(i=0; i<num_of_mallocs; i++) {
      x[i] = NULL;
    }
    for(i=0; i<num_of_mallocs; i++) {
      x[i] = (int*)malloc(100 * sizeof(int));
    }
    for(i=0; i<num_of_mallocs; i++) {
      if(x[i]) {
        free(x[i]);
      }
    }
  }
  return 0;
} 

int main() {
  //memproflite_start();
  const int num_of_threads = 1;
  pthread_t thread[num_of_threads];
  int index[num_of_threads];
  int i;
  for(i=0; i<num_of_threads; i++) {
    index[i] = i;
  }
  for(i=0; i<num_of_threads; i++) {
    pthread_create(&thread[i], NULL,abc, &index[i]);
  }
  for(i=0; i<num_of_threads; i++) {
    pthread_join(thread[i], NULL);
  }
  /*memproflite_stop(); */
}
