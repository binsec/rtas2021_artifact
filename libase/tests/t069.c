/* Il y avait un probleme ici de non-detection que a[0] etait egal, il
   une nouvelle variable etait créée pour lui indument. */
int a[2];
int u = 33;
main() {
  while (rand()) {
    a[1] = 33;
    u = a[0];
  }
}
