/* Tests reads and writes to different memory regions. */
void main(void){
  int a = 1;
  int b = 2;
  int *c = rand()? &a : &b;
  int d = *c;
  *c = 3;
  a;
  b;
}
