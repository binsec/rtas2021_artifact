/* Failure on path-sensitive analysis. */

struct {
  char a;
  char b;
  char;
  char ;
} c;
main() {
  * (int *) &c = rand();
  int d = 0, e = 0;
  switch (c.b)
  case 1:
  case 2:
    switch (c.a)
    case 1:
      while (d < 1) {
        if (d)
          if (d != e) {
          }
        d++;
      }
}
