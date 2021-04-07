/* This tests made the deserialization fault. */

int a[4];
int b, e;
int *c, *d;
main() {
  c = a;
  for (; b < 10; b++)
    e = *++c;
  d = c - 2;
  for (;;)
    *c-- = *d;
}
