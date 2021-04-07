/* The problem here is dereferencing null. */
void main (void)
{
  int *a = 0;
  int b = *a;
}
