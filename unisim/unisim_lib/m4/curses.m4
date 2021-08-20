## UNISIM_CHECK_CURSES
## Checks if the curses library is installed
#####################################################
AC_DEFUN([UNISIM_CHECK_CURSES], [
    # Check if curses path has been overloaded
    AC_ARG_WITH(ncurses,
	AS_HELP_STRING([--with-ncurses=<path>], [ncurses library to use (will be completed with /include and /lib)]))
    if test "x$with_ncurses" != "x"; then
		AC_MSG_NOTICE([using ncurses at $with_ncurses])
		LDFLAGS=${LDFLAGS}" -L$with_ncurses/lib"
		CPPFLAGS=${CPPFLAGS}" -I$with_ncurses/include"
    fi
	
    # Check for ncurses.h
    AC_CHECK_HEADER(ncurses.h, broken_ncurses=no, broken_ncurses=yes)

    # Check for functions wgetch in libncurses.a
    UNISIM_CHECK_LIB(ncurses, wgetch, broken_ncurses=no, broken_ncurses=yes)

    if test "$broken_ncurses" == "yes"; then
		AC_MSG_NOTICE([ncurses not found. No line edition capabilities will be available.])
    else
		LIBS="-lncurses ${LIBS}"
		AC_DEFINE([HAVE_NCURSES], [], [Whether ncurses is available])
    fi
])
