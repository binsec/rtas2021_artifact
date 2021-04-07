void __VERIFIER_assert(int);


/* This tests a useful implementation detail: when a condition c is
   reused, and we load the value v of a term t previously evaled with
   condition c, then v is loaded. Note however that terms derived from
   t may have been computed with conditions larger than c.

   This extra precision may be surprising, but also useful. */
void main(int u, int v)
{
  int i = 0;
  if( u * v <= 56) i = 1; else i = 2;
  int j = i+1;

  if( u * v <=  56){
    __VERIFIER_assert( u * v <= 56);
    __VERIFIER_assert( i == 1);
    __VERIFIER_assert( j == 2);
  }
}
  
