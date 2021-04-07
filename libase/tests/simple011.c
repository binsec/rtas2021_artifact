/* Each call to u() should return an unknown with a different id. */

int a;
int u(void);
void f(void){
  a = u();
  a = u();
}

void main(void)
{
  f();
  f();
}
  
