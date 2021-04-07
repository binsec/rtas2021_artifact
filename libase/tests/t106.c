/* Was showing a problem in the stack structure of abstract
   domains.  */
int a, b;
main() {
    b = 2;
  while (0 < 0) {
    a = 0;
    a++;
    a++;
    b = 2;
  }
}
