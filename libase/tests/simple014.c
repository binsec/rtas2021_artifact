/* In this test, we convert an unsigned value to a signed one using
   memcpy; the correct result for res is -3, and not 765. An integer
   model that does not handle "wrapping" should report an error
   here. Note that Frama-C option -warn-signed-overflow does not
   output any error here. */

void __VERIFIER_assert(int);

void main(void){
  unsigned char c = 255;
  char q = * (char *) &c;
  int res = q * 3;
  __VERIFIER_assert(res == -3);
  int res2 = c * 3;
  __VERIFIER_assert(res2 == 765);
}
