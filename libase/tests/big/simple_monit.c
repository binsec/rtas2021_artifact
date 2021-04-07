void __VERIFIER_assert(int);

#define CMD_BUFFER_SIZE 100
char command[CMD_BUFFER_SIZE];

#pragma noalign(command)
struct command {
  int len;
  short tag;
};

#pragma noalign(command_f0)
struct command_f0 {
  int len;
  short tag;
  int arg1;
  int arg2;
};

#pragma noalign(command_f1)
struct command_f1 {
  int len;
  short tag;
  int arg1;
  short arg2;
  int arg3;
};
  
void do_it(int arg);
  
void f0(int i,int len){                                         
  /* Frama_C_show_each_f0_i(i,len); */
  struct command_f0 *pCmd = (struct command_f0 *) &command[i];
  if(len != sizeof(struct command_f0)) return; /* Error. */

  /* Frama_C_show_each_f0(len,i,pCmd,&pCmd->arg1,&pCmd->arg2); */
  int arg1 = pCmd->arg1;
  int arg2 = pCmd->arg2;

  do_it(arg1 + arg2);
}

void f1(int i,int len){
  struct command_f1 *pCmd = (struct command_f1 *) &command[i];

  int *arg1 = &pCmd->arg1;
  short *arg2 = &pCmd->arg2;
  int *arg3 = &pCmd->arg3;

  /* Pointers are validated a posteriori. */
  /* THE BUG IS HERE, if I replace command_f0 with command_f1 */
   if(len != sizeof(struct command_f1)) return; /* Error. */

  /* Note: An interesting variant would be to check that len is longer
     than command_f1.  Could be solved using octagons. */
  int varg1 = *arg1;
  int varg2 = *arg2;
  int varg3 = *arg3;
  do_it(varg1 + varg2 + varg3);

}


void main(void){

  extern char getc(void);
  
  int i;
  for(i = 0; i < CMD_BUFFER_SIZE; i++) { command[i] = getc(); }

  i = 0;
  /* Note: in the real code, pCmd is strength-reduced. */
  while(1){
    struct command *pCmd = (struct command *) &command[i];

    /* Frama_C_show_each_i(i); */
    /* Frama_C_show_each(pCmd); */
    /* Error: not enough space in the buffer to read the length and tag. */
    if(i + sizeof(pCmd->len) + sizeof(pCmd -> tag) >= CMD_BUFFER_SIZE) break;

    /*@ assert i < CMD_BUFFER_SIZE - sizeof(struct command); */
    __VERIFIER_assert(i + sizeof(struct command) < CMD_BUFFER_SIZE);

    /*  We cannot remember that each pCmd->len is the same yet, so we
        introduce this temporary variable instead. */
    int len = pCmd->len;
    
    /* Error: not enough space to read the full command. */
    if(i + len >= CMD_BUFFER_SIZE) break;

    /* End of commands. */
    if(len == 0) break;

    /* Error: len < 0. */
    if(len < 0) break;

    switch(pCmd->tag){
    case 0: f0(i,len); break;
    case 1: f1(i,len); break;

    default: goto exit;
    }

    i += len;
  }

 exit:
  return;
}
