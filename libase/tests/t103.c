/* THe problem in this test seemed to be on nondet when the condition
   is false. */
void main(int d)
{
  if (! (1 < 0)) goto unrolling_5_loop;
  if (2 < 0) d++;
 label: ;
 unrolling_5_loop: ;
}
