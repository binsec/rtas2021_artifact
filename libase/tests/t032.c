/* This should be proved true by the SMT solver; but the translation
   did not handle tuples correctly. */
void __VERIFIER_assert(int);
void main(int u){
  int x,y;

  if(u) {x = 1; y = 1; } else {x = 2; y = 2;}
  __VERIFIER_assert(x == y);
}
