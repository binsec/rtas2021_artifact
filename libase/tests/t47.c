char b[8];
void main(void) {
  if(rand())
    *(int*)b = 3;
  else
    *(short *)(b + 2) = 7;
}
