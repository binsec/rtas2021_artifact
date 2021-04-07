/* Same as simple408, but with stores instead of loads. */
void main(int i,int j)
{
  int t[3] = {11,22,33};
  int c = 0;
  // if(i+j < 0 || i + j >= 3) while(1);
  t[i+j] = 1;
  t[i+j] = 2;
  int k = i;
  t[k+j] = 3;
}
