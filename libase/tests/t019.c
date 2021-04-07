/* This was a compilation bug; gave bottom at the end. */
void main(void) {
  int enabled = 1;
  if (enabled) {
  }
  enabled = enabled + 0;
}
