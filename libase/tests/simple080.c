/* Pointer substraction. */

void main(void){

  int a[8];
  int *q = &a[1];
  int *p = &a[4];
  int u = p - q;
  int v = p - a;
}
