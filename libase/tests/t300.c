/* Functions and nested loops. Was looping indefinitely. */
int e;
int b[1], c;
void fn1(int *p2) {
  for (; e;)
    *p2++;
}

main() {
  fn1(b);
  fn1(c);
}
