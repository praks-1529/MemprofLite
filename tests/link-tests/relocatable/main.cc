#include <iostream>
#include <memproflite.h>
#include <memproflite-static.h>

using namespace std;

void* abc(void* arg) {
  int i = 1;
  while(i--) {
    const int num_of_mallocs = 1;
    int *x[num_of_mallocs];
    for(int i=0; i<num_of_mallocs; i++) {
      x[i] = NULL;
    }
    for(int i=0; i<num_of_mallocs; i++) {
      x[i] = new int[100];
    }
    for(int i=0; i<num_of_mallocs; i++) {
      if(x[i]) {
        delete[] x[i];
      }
    }
  }
  return 0;
} 

int main() {
  memproflite_start();
  const int num_of_threads = 1;
  pthread_t thread[num_of_threads];
  int index[num_of_threads];
  for(int i=0; i<num_of_threads; i++) {
    index[i] = i;
  }
  for(int i=0; i<num_of_threads; i++) {
    pthread_create(&thread[i], NULL,abc, &index[i]);
  }
  for(int i=0; i<num_of_threads; i++) {
    pthread_join(thread[i], NULL);
  }
  memproflite_stop();
}
