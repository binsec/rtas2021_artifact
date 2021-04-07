void main(void){
  char c[30];
  int i = 3;
  while(i <= 18 && rand()) {i++; }
  char *ptr = &c[3];
  while(ptr < &c[28]) {ptr++; }
  ptr = &i;
  while(i <= 18 && rand()) {i++; ptr=&c[i];} 
  char *ptr2;
  while(i <= 18 && rand()) {i++; ptr2=&c[i];}
  int k;
  while(i <= 18) {i++; k = i; }
  Frama_C_show_each(i,ptr,ptr2,k);
}
  
