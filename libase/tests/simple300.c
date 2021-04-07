/* This tests that we are able to access the underlying representation
   of objects using casts to chars. It also checks that copying
   pointers works correctly. */
void main(int k)
{
  char *p = (char *) &k;
  char *q = p;
  char v = q[1];
}
