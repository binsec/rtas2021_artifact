/* Tests backward propagation of integers conditions. */

void __VERIFIER_assert(int cond);
/* TODO: Should be its own builtin. */
#define wrong_assert __VERIFIER_assert


void main(int u, int v, int w, int x)
{
  if( u < 3) {
    __VERIFIER_assert(u+1 < 4);
  }

  if( v <= 3) {
    __VERIFIER_assert(v+1 < 5);
  }

  if( w > 3){
    __VERIFIER_assert(w+1 > 4);
  }

  if( x >= 3){
    __VERIFIER_assert(x+1 > 3);
  }


  extern int unknown(void);
  u = unknown();
  v = unknown();
  w = unknown();
  x = unknown();

  if(!( u < 3)) {
    __VERIFIER_assert(u+1 >= 4);
    wrong_assert(u+1 >= 5);
  }

  if(!( v <= 3)) {
    __VERIFIER_assert(v+1 > 4);
  }


  if(!( w > 3)){
    __VERIFIER_assert(w+1 <= 4);
  }

  if(!( x >= 3)){
    __VERIFIER_assert(x+1 <= 5);
  }

}
