/* The bug here, was that:

   - The if statement is incorrect, and the returned memory state is bottom;
   - The With_Bottom functor replaced bottom by the memory element at the beginning of the bloc
   - The Finite_Memory functor introduced the element again, because it could not know that
     it was already introduced.
     The solution was to have a cache representing for each location,
     if the corresponding binary was introduced (and which it was).  */
int a, b, c;
main() {
  int d = &c;
  a = 3;
  while (1) {
    a++;
    if (b < d)
      goto return_label;
    a++;
  }
 return_label:;
}
