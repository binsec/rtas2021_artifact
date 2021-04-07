/* This tested that we do not call "intro" when the region does not
   change in both branches of the if. */

int quant26bt_neg[31] = { 
  3, 2, 1, 0, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, 9,
  8, 7, 6, 5, 4, 4
};
int quantl_mil;


extern int u(void);

int main_i;
main ( )
{
  int a = u();
  if( a % 5 != 3) while(1);
  quant26bt_neg[30] = a;
  quant26bt_neg[29] = a;  
  
  
  while ( main_i < 10 )
    {
      quantl_mil = u();
      if(quantl_mil >= 29) break;
      if ( u())
        quant26bt_neg[quantl_mil];
      main_i += 2;
    }
}
