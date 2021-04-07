/* Tests nested loops: reading "a" in the inner loop also requires
   adding "a" in the outer loop. */
main() {
  int a = 0;
  int i, j;
  for (i = 0; i < 100; i++)
    for (j = 0; j < 1000; j++) a++;
}
