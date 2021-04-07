/* This example found a bug where an "assume" did not take as a
   "minimal condition" the conjunction of the conditions of its
   arguments. */
main(int i) {
  if (!i)
    goto unrolling_15_loop;
  if (i < 2)
    while (i)
      i++;
 unrolling_2_loop:
  ;
 unrolling_15_loop:;
}
