char a[20];
struct command {
  int len;
  short tag;
};
int b;
main() {
  b = rand();
  struct command *c = &a[b];
  c->tag;
}
