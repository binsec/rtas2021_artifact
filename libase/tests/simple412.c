/* This shows inter-statement backward propagation.  */

/*@ requires cond == 1; 
    assigns \nothing; */
void __VERIFIER_assert(int cond);

void main(int u, int v, int w)
{
  u = u;
  v = v;
  w = w;
  while(!(u < 1000 && v < 1000 && w < 1000));
  if(v + 1 < 3) __VERIFIER_assert(v < 2);
  
  int tmp = u;
  if(tmp + 1 < 3) __VERIFIER_assert(u < 2);

  int tmp2 = w + 1;
  if(tmp2 < 3) __VERIFIER_assert(w < 2);
}
