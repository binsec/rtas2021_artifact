/* Note: similar to t107, but tests something different. */
int ar[2];
extern int fn2(void);

void main(void) {
  int u,v;
  int h = fn2();
  if(h)
    {
      unsigned a = 0;
      while (a < 10)
        {
          a++;
          u = ar[h];
      }
    }
  else
    v = ar[0];
}

