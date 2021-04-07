/* Test a forward reevaluation of i+6. */
void main(int i){
  if(i + 6 > 444) while(1);  
  if(i > 12) while(1);//return;
  Frama_C_show_each_f0(i+5,i+6,i+7);
}
