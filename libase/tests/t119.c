char c[2];  
void main(void) {
  short rand(void);
  *(short *)&c = rand();

  if (c[0] == 3)
    if (c[1]) {
    }
}
