extern  __attribute__((__FC_BUILTIN__)) void Frama_C_bzero(unsigned char *dest,
                                                           unsigned long n);


void main (void)
{
  char sk[32];
  Frama_C_bzero((unsigned char *)(sk),(unsigned long)sizeof(char [32]));
}

