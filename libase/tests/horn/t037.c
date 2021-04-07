/* Le probleme semble etre qu'il manquait la condition dans la query */
void main() {
  int a = u();
  if (a)
    goto switch_7_exp_0;
  goto switch_7_exp_8;
  {
  switch_7_exp_0:
    goto switch_7_break;
  switch_7_exp_3:
    ;
  switch_7_exp_8:
    ;
  }
switch_7_break:
  __VERIFIER_error();
}
