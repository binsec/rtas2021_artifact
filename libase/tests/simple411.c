void __VERIFIER_assert(int);

/* This tests backward propagation of addition. */
void main(int c)
{
  int j = c + 4;
  if(j > 11)
    __VERIFIER_assert(c > 7);
  else while(1);
}

