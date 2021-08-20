#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct
{
	char *searchFor;
	char *replaceBy;
} conversion_table[] = {
	//"::", "::",
	//":", "\\hspace{-5pt}:",
	"#", "\\#",
	" ", "~",
	"_", "\\_",
	"\t", "~~",
	"{", "$\\{$",
	"}", "$\\}$",
	"&", "\\&",
	"--", "{-}{-}",
//	"\n", "}\\\\\n\\texttt{",
	"<<", "<}\\texttt{<",
	"<<", ">}\\texttt{>",
	"%", "\\%",
	"//(1)", "\\ding{202}",
	"//(2)", "\\ding{203}",
	"//(3)", "\\ding{204}",
	"//(4)", "\\ding{205}",
	"//(5)", "\\ding{206}",
	"//(6)", "\\ding{207}",
	"//(7)", "\\ding{208}",
	"//(8)", "\\ding{209}",
	"//(9)", "\\ding{210}",
	"/*(1)*/", "\\ding{202}",
	"/*(2)*/", "\\ding{203}",
	"/*(3)*/", "\\ding{204}",
	"/*(4)*/", "\\ding{205}",
	"/*(5)*/", "\\ding{206}",
	"/*(6)*/", "\\ding{207}",
	"/*(7)*/", "\\ding{208}",
	"/*(8)*/", "\\ding{209}",
	"/*(9)*/", "\\ding{210}"
};

int convert(char *inputfilename, char *outputfilename, int nlines, int line_width)
{
	FILE *file = fopen(inputfilename, "rb");
	char *buffer, *ptr;
	int searchForLength;
	int replaceByLength;
	long filesize;
	size_t n;
	int lineno = 0;
	int colno = 0;
	char commandLine[256];

	if(!file)
	{
		fprintf(stderr, "Can't open %s for reading...\n", inputfilename);
		return -1;
	}

	if(fseek(file, 0, SEEK_END) == -1 ||
		(filesize = ftell(file)) == -1 ||
		fseek(file, 0, SEEK_SET) == -1)
	{
		fprintf(stderr, "Can't determine size of %s...\n", inputfilename);
		return -1;
	}

	buffer = (char *) malloc(filesize);
	if(!buffer)
	{	
		fprintf(stderr, "Not enough memory...\n");
		fclose(file);
		return -1;
	}

	printf("Reading %ld bytes from %s...\n", filesize, inputfilename);

	if((n = fread(buffer, 1, filesize, file)) != filesize)
	{
		fprintf(stderr, "Can't read into %s...\n", inputfilename);
		fclose(file);
		return -1;
	}

	fclose(file);
	
	file = fopen(outputfilename, "wb");

	if(!file)
	{
		fprintf(stderr, "Can't open %s for writing...\n", outputfilename);
		return -1;
	}

	printf("Writing into %s...\n", outputfilename);

	ptr = buffer;

	while(filesize > 0)
	{	
		//fprintf(file, "\\begin{figure}\n");
		//fprintf(file, "\\clearpage\n");
		fprintf(file, "\\begin{center}\n");
		fprintf(file, "\\scriptsize\n");
//		fprintf(file, "\\begin{tabular}{|l|}\n");
		fprintf(file, "\\begin{tabular}{|p{15.5cm}|}\n");
		fprintf(file, "\\hline\n");
//		fprintf(file, "\\hspace{15.5cm}~\n");
		fprintf(file, "\\texttt{");
	
		lineno = 0;
		colno = 1;
		int new_line = 1;
		int line_break = 0;
		
		while(filesize > 0 && lineno < nlines)
		{
			int i;
			int found = 0;
			if(new_line)
			{
				lineno++;
				colno = 1;
				fprintf(file, "}\\\\\n\\texttt{%2d~", lineno);
			}
			if(line_break)
			{
				fprintf(file, "}\\\\\n\\texttt{$\\hookrightarrow$~~~~");
				colno = 1;
			}
			
			new_line = strncmp(ptr, "\n", strlen("\n")) == 0;
			
			line_break = colno > line_width && isblank(*ptr);
			
			for(i = 0; i < sizeof(conversion_table)/sizeof(conversion_table[0]); i++)
			{
				if(strncmp(ptr, conversion_table[i].searchFor, strlen(conversion_table[i].searchFor)) == 0)
				{
					fwrite(conversion_table[i].replaceBy, strlen(conversion_table[i].replaceBy), 1, file);
					ptr += strlen(conversion_table[i].searchFor);
					colno += strlen(conversion_table[i].searchFor);
					filesize -= strlen(conversion_table[i].searchFor);
					found = 1;
					break;
				}
			}
			
			if(!found)
			{
				fputc(*ptr, file);
				ptr++;
				colno++;
				filesize--;
			}
			
		}
		
		fprintf(file, "}\\\\\n");
		fprintf(file, "\\hline\n");
		fprintf(file, "\\end{tabular}\n");
		fprintf(file, "\\end{center}\n");
		//fprintf(file, "\\caption{\\label{fig:?} ?}\n");
		//fprintf(file, "\\end{figure}");
	}

	free(buffer);

	fclose(file);
	return 0;
}

int main(int argc, char *argv[])
{
	if(argc > 2)
	{
		convert(argv[1], argv[2], argc > 3 ? atoi(argv[3]) : 10000, argc > 4 ? atoi(argv[4]) : 10000);
	}
	else
	{
		fprintf(stderr, "Usage : c_to_tex <input file> <output file>\n");
	}
	
	return 0;
}
