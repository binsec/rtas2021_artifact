void __VERIFIER_assert(int);

/* Tests the progressive widening. */
void main(void){
  int a[10] = {0,0,0,0,0,0,0,0,0,0};

  /* Currently does not work: the first values of a[0] and a[1] are widened to +oo.
     For this to work, we should find the correct fixpoint for i before widening 
     the values in a. */
  for(int i = 0; i < 10; i++) a[i] = i;

  __VERIFIER_assert(a[0] < 10);
  __VERIFIER_assert(a[1] < 10);
  __VERIFIER_assert(a[2] < 10);
  __VERIFIER_assert(a[3] < 10);
  __VERIFIER_assert(a[4] < 10);
  __VERIFIER_assert(a[5] < 10);
  __VERIFIER_assert(a[6] < 10);
  __VERIFIER_assert(a[7] < 10);
  __VERIFIER_assert(a[8] < 10);
  __VERIFIER_assert(a[9] < 10);
}
