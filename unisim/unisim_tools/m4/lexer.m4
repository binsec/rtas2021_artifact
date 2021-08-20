## UNISIM_CHECK_LEXER_GENERATOR
## Checks if a lexer generator is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_LEXER_GENERATOR], [
	AC_PROG_LEX

	AC_CHECK_PROG(lexer_generator_installed, $LEX, yes, no)
	if test "x$lexer_generator_installed" != "xyes"; then
		AC_MSG_ERROR([No lexer generator found. Please install a lexer generator (either flex or lex).])
	fi

	if test "$LEX" != flex; then
		LEX="$SHELL $missing_dir/missing flex"
		AC_SUBST([LEX_OUTPUT_ROOT], [lex.yy])
		AC_SUBST([LEXLIB], [''])
	fi
])
