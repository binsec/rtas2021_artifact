int *a;
char e[17];

extern int u();

void main(void) {
  int b = 0;
  while (b < 10) {
    b++;
    a = e;
    int bytes = u();
    if(bytes == 4)
      bytes = (*a | *(a + 3));
  }
}
