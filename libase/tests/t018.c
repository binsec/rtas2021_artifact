/* This tests non-contiguous modifications of a memory region. */
int rand(void);

#define SIZE 4

void main(void) {
  unsigned char board[SIZE][SIZE];
  while (1) {
    if (rand()) {
      unsigned char x;
      for (x = 0; x < SIZE; x++) {
        board[x][0] = 3;
      }
    }
    /* The bug was here. */
    if (rand())
      break;
  }
}
