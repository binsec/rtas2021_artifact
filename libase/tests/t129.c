typedef unsigned char uint8_t;

void main(void){
  uint8_t count = 0;
  while(rand()) {
    count = (uint8_t)((int)count + 1);
    int count2 = count;
    if (count2 % 5 == 1) { Frama_C_show_eachthen(count2,count); } else { Frama_C_show_eachelse(count2,count); }
    Frama_C_show_each3(count);
  }
}
