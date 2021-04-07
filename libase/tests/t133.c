/* Not found exception when converting to Horn clauses. */
void main(void) {
  int a = 5;
  int b = 0;
  while (1)
    if (a == 5) {
      a = 6;
      b = a;
      if (rand())
        __VERIFIER_error();
    }


}
