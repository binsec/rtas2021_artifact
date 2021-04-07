/* Problem due to the fact that the value is changed in the same way in both cases. */
void  main(int d, int e, int c) {
  if (e) {
    if (d)
      c |= 1;
    else
      c |= 1;
    c++;
  }
}
