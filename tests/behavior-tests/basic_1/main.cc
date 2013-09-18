#include <iostream>
#include <memproflite.h>

using namespace std;

int* def() {
  int *x = new int[100];
  return x;
}

void abc() {
  int *x = def();
  free(x);
} 

int main() {
  abc();
}
