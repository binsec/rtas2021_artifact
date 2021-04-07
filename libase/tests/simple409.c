/* The use of decreasing iteration sequences requires to evaluate the
   alarms at the end. */
void main(void)
{
  int t[111];
  int n = 110;
  int i = 0;
  for(;;){
    /* No alarm here. */
    t[i] = 1;
    i++;
    if(i > n) break;
  }
}
