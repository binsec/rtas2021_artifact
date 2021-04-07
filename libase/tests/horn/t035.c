/* SMT said UNSAT, but it was SAT. */
void __VERIFIER_error(void);

void main(_Bool k) {
  int a = 0;
  int b = 2;
  if (k)
    a = 2;
  int p1 = (b < 5);
  if (p1)
  if(b < 5)
    __VERIFIER_error();
}

