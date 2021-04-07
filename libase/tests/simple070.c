/* Demo: low-level memory model + weakest-precondition generation + weak updates */
/* Actually, the cast to short is incorrect; what is correct is the same thing with casts to char*. */
void __VERIFIER_assert(int);

void main(int b){
  /* So that we can observe both values. */
  if(!((b % 16 == 3) && (b < (1 << 18)))) while(1);
  int old = b;
  short *p = &b;
  short low = *p;
  short high = *(p+1);
  /* *p = high; */
  /* *(p+1) = low; */
  int new;
  p = &new;
  *p = low;
  *(p+1) = high;
  int new_read =  new;
  int new2;
  short *q = &new2;
  *q = high;
  *(q+1) = low;
  int new2_read = new2;
  __VERIFIER_assert(new == old);
  __VERIFIER_assert((new2 >> 16) == (old & 0xFFFF));
  __VERIFIER_assert((new2 & 0xFFFF) == (old >> 16));
}
