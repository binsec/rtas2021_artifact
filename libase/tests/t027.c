/* The bug was in the comparison &main_str; the ref_addr domain said
   false, but the integer domains said true, and the intersection was
   bottom. */
char main_str;
int main_i = -1;
int main_j;
main (  )
{
  do
    {
      main_i++;
      main_j = main_i - 1;
      if ( 0 <= main_j )
	__VERIFIER_error (  );
    }
  while ( &main_str );
}
