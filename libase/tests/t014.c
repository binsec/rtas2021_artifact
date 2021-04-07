/* This tests induced a "broken Top" failure in Ival. */
main(int e) {
  int a = 0;
  if(e) a = -2;
  if (a)
    a = 13;
}
