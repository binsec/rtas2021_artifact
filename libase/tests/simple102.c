void __VERIFIER_assert(int);

/* In this test, j is only written to, and not read, and is not
   strictly speaking part of the fixpoint. The test checks that it is
   still written at the end of the loop. */
void main(void){
  int i,j;
  for(i = 0; i < 10; i++) {
    j = i;
  }
  __VERIFIER_assert(j != 0);
}
