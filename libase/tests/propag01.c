void __VERIFIER_assert(int);

void main(int u, int v){
  /* To find that u \in {67}:
     After the first test, we know that u + v \in {100} (backward propagation)
     After the second, we know that v \in {33}; 
     then we must do a _backward_ propagation on u + v (which is a
     _parent_ of v) to learn that u \in {67}  */
  if(!(u + v == 100)) while(1);
  Frama_C_show_each_u1(u,v,u+v);
  if(!(v == 33)) while(1);
  Frama_C_show_each_u2(u,v,u+v);
  __VERIFIER_assert(u == 67);
}
