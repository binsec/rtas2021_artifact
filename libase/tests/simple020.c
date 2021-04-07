/* Tests statements that have no successors. */
extern __attribute__ ( ( __noreturn__ ) )
     void exit ( int status );

void main(void){
  exit(0);
}
