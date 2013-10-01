/*!****************************************************************************
 *
 *  \file       main.cc
 *
 *  \brief
 *      
 *
 *  \details
 *
 *  \internal
 *  HISTORY:
 *      09/11/2013 - Prakash S : Created
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>

class A {
  int x;
  char txt[6];
};

class C {
  int x;
};

class B : public A {
  public:
    B(void) {
     self_p = new C;
    }
    ~B(void) {
      delete self_p;
    }
    int b;
    C* self_p;
    C self;
    char txt[10];
};

int main() {
  printf("A size = %ld\n", sizeof(A)); // 12
  printf("B size = %ld\n", sizeof(B)); // 40
  printf("C size = %ld\n", sizeof(C)); // 4
  B *b = new B;
  delete b;
}
