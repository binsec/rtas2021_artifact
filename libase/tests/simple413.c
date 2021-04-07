/* This tests that overlapping writes to unknown locations are handled correctly. */
void main(int i,int j){
  char a[] = {1,2,3,4,5,6,7};
  short *p = &a[i];
  *p = 0x0800;
  a[0];
  a[1];
  a[2];
  a[3];
  a[4];
  a[5];
  a[6];
  a[j];
}
