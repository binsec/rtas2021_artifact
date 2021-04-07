/* The problem was that both calls to bunknown used the same trace
   argument. */
double sqrt();
float fabsf();
int a,b;

void main(int u) {
  if(u)
    a = fabsf();
  else
    b = sqrt();
}
