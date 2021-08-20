## UNISIM_CHECK_TERMCAP
## Checks if the termcap library is installed
#####################################################
AC_DEFUN([UNISIM_CHECK_TERMCAP], [
    # Check if termcap path has been overloaded
    AC_ARG_WITH(termcap,
	AS_HELP_STRING([--with-termcap=<path>], [termcap library to use (will be completed with /include and /lib)]))
    if test "x$with_termcap" != "x"; then
	AC_MSG_NOTICE([using termcap at $with_termcap])
	LDFLAGS=${LDFLAGS}" -L$with_termcap/lib"
	CPPFLAGS=${CPPFLAGS}" -I$with_termcap/include"
    fi
	
    # Check for termcap.h
    AC_CHECK_HEADER(termcap.h, broken_termcap=no, broken_termcap=yes)

    # Check for functions in termcap
    UNISIM_CHECK_LIB(termcap, tgoto,
    UNISIM_CHECK_LIB(termcap, tputs,
    UNISIM_CHECK_LIB(termcap, tgetstr,
    UNISIM_CHECK_LIB(termcap, tgetent,
    UNISIM_CHECK_LIB(termcap, tgetflag,
    UNISIM_CHECK_LIB(termcap, tgetnum, broken_termcap=no,
    broken_termcap=yes),
    broken_termcap=yes),
    broken_termcap=yes),
    broken_termcap=yes),
    broken_termcap=yes),
    broken_libedit=yes)
	
    if test "$broken_termcap" == "yes"; then
		AC_MSG_NOTICE([termcap not found. No line edition capabilities will be available.])
    else
		LIBS="-ltermcap ${LIBS}"
		AC_DEFINE([HAVE_TERMCAP], [], [Whether termcap is available])
    fi
])
