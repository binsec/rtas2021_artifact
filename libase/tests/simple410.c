/* This tests the failure of eager evaluation when new subterms can be
   created: a term corresponding to (a+b) is created by the
   computation of e, which did not exist previously. */
void main(int a, int b)
{
  int c = 2 + a;
  int d = b + 3;
  int e = c + d;
  if(e < 4) while(1);
}
