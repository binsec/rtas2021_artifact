#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>

void uname_test()
{
	struct utsname buf;
    int uname_ret;

	printf(
"***********************************************************************\n"
);
	printf(
"START TEST: 'uname' system call translation ***************************\n"
);
	uname_ret = uname(&buf);
	printf("Uname result: %i\n", uname_ret);
	if ( uname_ret == 0 )
	{
		printf(" - sysname = %s\n", buf.sysname);
		printf(" - nodename = %s\n", buf.nodename);
		printf(" - release = %s\n", buf.release);
		printf(" - version = %s\n", buf.version);
		printf(" - machine = %s\n", buf.machine);
	}
	printf(
"END TEST: 'uname' system call translation ******************************\n"
);
	printf(
"************************************************************************\n"
);
}

int rec_fibonnaci(int n)
{
	printf(".");
	fflush(stdout);
	if ( n == 1 ) return 0;
	if ( n == 2 ) return 1;
	return rec_fibonnaci(n-1) + rec_fibonnaci(n-2);
}

void rec_fibonnaci_test()
{
	int goal = 12;
	int result = 0;
	printf(
"***********************************************************************\n"
);
	printf(
"START TEST: 'recursive fibonnaci' *************************************\n"
);
	printf("Computing fibonnaci of %d recursively.\n", goal);
	printf("Progress: ");
	result = rec_fibonnaci(goal);
	printf("\n");
	printf("Fibonnaci(%d) = %d\n", goal, result);
	printf(
"END TEST: 'recursive fibonnaci' ***************************************\n"
);
	printf(
"***********************************************************************\n"
);

}

void matrix_test()
{
	unsigned long long int **matrix_a;
	int i, j, k;
	unsigned long long int sum;
	unsigned int size = 200;

	printf(
"***********************************************************************\n"
);
	printf(
"START TEST: 'matrix test' *********************************************\n"
);

	printf("Initializing matrix");
	matrix_a = (unsigned long long int **)malloc(sizeof(unsigned long long int) * size);
	for ( i = 0; i < size; i++ )
	{
		matrix_a[i] = (unsigned long long int *)malloc(sizeof(unsigned long long int) * size);
		for ( j = 0; j < size; j++ )
		{
			matrix_a[i][j] = 0;
		}
		printf(".");
	}
	matrix_a[0][0] = 1;
	printf("\n");
	

	printf("Computing result");
	for ( i = 0; i < size; i++ )
	{
		for ( j = 0; j < size; j++ )
		{
			sum = 0;
			for ( k = 1; k < i; k++ )
			{
				sum += matrix_a[k][j];
			}
			for ( k = 1; k < j; k++ )
			{
				sum += matrix_a[i][k];
			}
			if ( (i != 0) || (j != 0) )
				matrix_a[i][j] = sum;
		}
		printf(".");
		fflush(stdout);
	}
	printf("\n");
	printf("Releasing matrix");
	for ( i = 0; i < size; i++ )
	{
		free(matrix_a[i]);
		printf(".");
	}
	free(matrix_a);
	printf("\n");
	printf(
"END TEST: 'matrix test' ***********************************************\n"
);
	printf(
"***********************************************************************\n"
);
}

int main(int argc, char *argv[])
{
	printf(
"ARMEmu simulator selftest =============================================\n"
);
	printf("\tAuthor: daniel.gracia-perez@cea.fr\n");
	printf("\tLast update: 27/08/2010\n");
	printf(
"=======================================================================\n"
"\n\n");

	uname_test();
	printf("\n");

	rec_fibonnaci_test();
	printf("\n");

	matrix_test();
	printf("\n");

	printf(
"End of ARMEmu simulator selftest ======================================\n"
);
	printf(
"=======================================================================\n"
);


	return 0;
}
