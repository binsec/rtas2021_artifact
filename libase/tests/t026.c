void main(void){
  char a[3],b[3];

  int res;
  switch(rand()){
  case 0: res = &a[0] == &b[0]; break;
  case 7: res = &a[0] == &b[1]; break;    
  case 1: res = &a[0] <= &a[1]; break;
  case 2: res = &a[1] <= &a[0]; break;
  case 3: res = &a[0] <= &b[1]; break;
  case 4: res = &a[1] <= &b[0]; break;
  }
  
}
