struct s {
   int *spare ;
   int deck1[7] ;
};
int a;
int deck2[7];
struct s state;
int *state_2;
int main_kval;
extern int rand(void);

void main(void)
{
  state_2 = & state.deck1[3];
  state_2 = state_2;
  state.spare = & deck2[3];
  while (rand()) {
    int b = a;
    a = state_2[b];
    a = a;
    b = b;
    main_kval = rand();
    a = ((a + 3) - main_kval) % 53;
    state_2 = &deck2[0];
  }
  return;
}

