void __VERIFIER_assert(int);

/* Tests a simple loop. */
void main(int c){
  int i = 0;
  int n = 28+3;
  while(i < n) {i++;}

  __VERIFIER_assert(i == 31);

  int j = 0;
  if(c) n = 100;
  while(j < n) {j++;}
  __VERIFIER_assert(j <= 100);
}
