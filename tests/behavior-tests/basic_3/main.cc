#include <iostream>
#include <memproflite.h>
#include <stdlib.h>
using namespace std;

void def() {
  int *x = new int[100];
  /* Intentional leak */
  //free(x);
}

void abc() {
  def();
} 

int main() {
  int *x;
  x = new int[100];
  abc();
  def();
  free(x);
}
