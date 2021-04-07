/* Handling completely invalid pointers.  */
void main(int c) {
  int a[1] = {3};
  int i;
  switch(c) {
  case 0:
    i = a[2];
    break;
  case 1:
    a[2] = 1;
    break;
  case 2:
    a[-1] = 3;
    break;
  default: while(1);
  }
}
