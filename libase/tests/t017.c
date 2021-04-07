/* The problem with that one was that (deck1 + 3) + 3 got rewritten
   into (deck1 + 6), but no value was given to "6", which made
   backward evaluation fail.  */

int deck1[7];
int *b;
void main(void) {
  b = deck1 + 3;
  b[3];
}
