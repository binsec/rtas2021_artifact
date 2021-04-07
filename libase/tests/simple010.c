/* Tests initial value of global variables. */

void __VERIFIER_assert(int);

int i = 2;
int j;
int k[8] = {11,22,33};

void main(void)
{
  int q = k[i] + k[j] + k[6];
  __VERIFIER_assert(q == 44);
}
