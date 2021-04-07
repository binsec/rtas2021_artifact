/* I had a bug here: we can point to an array element
 (especially when a char) and cast to a larger type. */
char command[8];

struct command {
  int len;
  short tag;
};

void main(void){
    struct command *pCmd = (struct command *) &command[0];
    int len = pCmd->len;
    len ++;
}
