/* C does not mandate that both operands of >> have the same size;
   this test checks that this is handled correctly. */
unsigned char z_0;
long long r;
long long i;

void main ( void )
{
  r = z_0 >> i;
}
