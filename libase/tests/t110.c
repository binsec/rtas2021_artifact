/* Failure on path-sensitive analysis. */
struct {
  char a;
  char b;
  char c;
  char e;
} d;

main() {
  int e = rand();
  int f = 0;
  int g = 0;
  * (int *) &d = rand();

  d.a = 0;
  d.e = 0;
  
  switch (d.c) {
  case 11:
  case 22:
    switch (d.b)
    case 1: {
      int i = 0;
      while (f < 1) {
        if (f)
          i = 1;
        if (i == 1) {
          if (f != g) {
            e = g;
            __VERIFIER_assume(e < 1);
          }
          g++;
        }
        f++;
      }
    }
}
}
