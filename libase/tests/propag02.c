/* This triggered an infinite loop. */

void main(int u){
  if( u > 33) while(1);
  Frama_C_show_each33(u);
  if( u < 11) while(1);
  Frama_C_show_each11(u);       /* Result is 11..33 */
}
