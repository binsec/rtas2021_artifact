/* This caused a Never_refined bug. */
void main(void) {
  int e = rand();
  int d = rand();
  int b = rand();
  if (e)
    while (d);
  int tmp = b == 1 != 0;
}
