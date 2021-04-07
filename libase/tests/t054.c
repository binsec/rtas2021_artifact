char decompressed[1];
char *u = 0;
char *dst;

void main(void) {
  dst = decompressed;
  while (rand()) {
    dst += 1;
    while (decompressed < dst - 1)
      u = decompressed;
  }
}
