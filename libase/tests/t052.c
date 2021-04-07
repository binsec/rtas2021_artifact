void main(void) {

  if(rand()) goto r;

  {
    int a = 3;
    a--;
    goto b;
  r: a = 4;
    goto b;
  b:
    a++;
  }
}
