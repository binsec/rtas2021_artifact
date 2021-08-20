## UNISIM_CHECK_PARSER_GENERATOR
## Checks if a parser generator is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_PARSER_GENERATOR], [
	AC_PROG_YACC

	AC_CHECK_PROG(parser_generator_installed, $YACC, yes, no)
	if test "x$parser_generator_installed" != "xyes"; then
		AC_MSG_ERROR([No parser generator found. Please install a parser generator (either bison, byacc or yacc).])
	fi
])
