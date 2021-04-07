/* There was a problem in the way we generated bitfield decoding
   instructions. */
struct {
  int BYTE7 : 8;
} a;
int b, d;
char c;
void main(void) {
  a.BYTE7 = c;
  c = a.BYTE7;
  d = b;
}
