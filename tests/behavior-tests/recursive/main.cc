void recur(int i) {
  if(i == 10) {
    return;
  } else {
     int *x = new int[10];
     recur(i+1);
     delete x;
  }
}

int main() {
  recur(0);
}
