typedef int word_t;

word_t
gcd( word_t a, word_t b )
{
  if (a < 0) a = -a;
  if (b < 0) b = -b;
  if (a < b) { word_t tmp = a; a = b; b = tmp; }

  while (b != 0)
    {
      word_t mod = a;
      while (mod >= b)
        mod -= b;
      a = b;
      b = mod;
    }

  return a;
}

