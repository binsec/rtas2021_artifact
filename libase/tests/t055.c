/* The size of what is modified differs between successive iterations,
   which must be handleed in the constraint domain. */
struct {
  short TC_word;
  unsigned;
} b, d;
int a = 0;
main() {
  while (rand()) {
    a = a + 1;
    d.TC_word = 7;
    b.TC_word = 0;
    if (a % 2)
      b = d;
  }
}
