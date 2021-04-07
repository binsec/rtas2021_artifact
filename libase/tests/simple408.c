void __VERIFIER_assert(int);

/* Tests that redundant alarms are removed; here there is only one
   alarm for reading t[i], the others are removed. */
void main(int i,int j)
{
  int t[3] = {0,0,0};
  int c = 0;
  // if(i+j < 0 || i + j >= 3) while(1);
  c = t[i+j] + 1;
  c = t[i+j] * 2;
  int k = i;
  c = t[k+j] + 3;
}

