void __VERIFIER_assert(int);

/* Very simple conditional, with intervals. */
void main(int u)
{
  int t = 22;
  if(u > 3) { t = 33; }

  __VERIFIER_assert( t == 22);
  __VERIFIER_assert( t == 33);
  __VERIFIER_assert( 22 <= t);
  __VERIFIER_assert( t <= 33);
}
