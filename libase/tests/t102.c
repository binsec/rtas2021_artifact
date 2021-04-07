/* The thing it tests here is that e is written to only in one branch;
   when merging, the value of e in the "else" branch has to be read,
   and for this a new binary must be introduced in the context. */
void main(int a, int c, int e) {
  while(a) {
    if (c) e = 1;
  }
}
