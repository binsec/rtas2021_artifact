void __VERIFIER_assert(int);

/* This check that even without backward propagation, we still
   "remember" when something is true. Here the conditions are
   sufficiently complicated that the backward propagation stops
   immediately. */
void main(int u,int v){

  int a = 11;
  
  if(u+v == 17){
    if(u+v != 17) a = 22;
  }
  __VERIFIER_assert(a == 11);


  int b = 33;
  if(u*v == 32) while(1);
  if(u*v == 32) { b = 44; } else { b = 55; }
  __VERIFIER_assert(b == 55);
  __VERIFIER_assert(!(u*v == 32));
}
