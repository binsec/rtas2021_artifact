/* Proving that c = 33 in the last expression is hard. The beginning
   establishes that c == 33 for the condition "b == 0 || b == 4". But
   it is difficult to find out that the result holds also for "b == 4"
   alone. */

void main(int b, int c) {
  if (b == 0)
    goto l;
  if (b == 4) {                 /* Or just: if (b==4 || b == 0)... */
  l: if (c!=33) while(1);
  }
  if (b == 4) {
    c = c + 1; // c == 33 in this expression.
  }
}
