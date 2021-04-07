/* This tests the values of divisions. */

void __VERIFIER_assert(int);

void main(int u){
  int v = u * 5 + 2;
  //  if(!(v < 0 && v > - 8)) while(1);
  int q,r,s,t;

  /* Truncated divisions. */
  s = v % 5;
  if(v < 0) q = v % 5;
  else r = v % 5;

  __VERIFIER_assert(q == -3);
  __VERIFIER_assert(r == 2);
  __VERIFIER_assert(s == 2); // False

  /* Euclidian divisions */
  int a = u * 16 + 3;
  int b = a & 15;
  int c = a >> 4 ;

  __VERIFIER_assert(a == 3);

  int d,e,f,g;
  if(u % 5 == 2) e = u;
  if(u % 5 == -2) f = u;

  /* Remainder positive implies e > 0. */
  __VERIFIER_assert(e > 0);
  __VERIFIER_assert(f < 0);


  /* Test against some constants */
  __VERIFIER_assert( 17 /  5 ==  3);
  __VERIFIER_assert( 17 %  5 ==  2);
  __VERIFIER_assert( 17 / -5 == -3);
  __VERIFIER_assert( 17 % -5 ==  2);
  __VERIFIER_assert(-17 /  5 == -3);
  __VERIFIER_assert(-17 %  5 == -2);
  __VERIFIER_assert(-17 / -5 ==  3);
  __VERIFIER_assert(-17 % -5 == -2);

  int ww = -17 / -5;
  int ww2 = -17 % -5;
}
