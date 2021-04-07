char board[3][2];

main() {
  int y = rand();
  char *p = &board[1];
  p[y] = 2;

  int r;
  r = board[0][0];
  r = board[0][1];
  r = board[1][0];
  r = board[1][1];
  r = board[2][0];
  r = board[2][1];
  board[0][0] = 00;
  board[0][1] = 11;
  board[1][0] = 22;
  board[1][1] = 33;
  board[2][0] = 44;
  board[2][1] = 55;

  int z = rand();
  char *p2 = &board[1];
  int r2 = p2[z];
}
