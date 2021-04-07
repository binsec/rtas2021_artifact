  unsigned char LifeSig_Buffer_CircBuff[4];
  unsigned char LifeSig_Period_f[4];
void main() {
  * (int *) LifeSig_Period_f = rand();
  * (int *) LifeSig_Buffer_CircBuff = rand();
  int b = rand();
  int c,d;
  d = 4 - b;
  c = 0; /*@loop pragma UNROLL 8;    */
  while (c < d) {
    LifeSig_Period_f[c] = LifeSig_Buffer_CircBuff[b + c];
    c++;
  }
}
