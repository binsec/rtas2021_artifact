## UNISIM_CHECK_LIBEDIT
## Checks if the libedit library (Port of NetBSD alternative 'editline' to GNU GPL 'readline') is installed
## Takes one parameter: the main function name
#####################################################
AC_DEFUN([UNISIM_CHECK_LIBEDIT], [
    # Check if libedit path has been overloaded
    AC_ARG_WITH(libedit,
	AS_HELP_STRING([--with-libedit=<path>], [libedit library to use (will be completed with /include and /lib)]))
    if test "x$with_libedit" != "x"; then
	AC_MSG_NOTICE([using libedit at $with_libedit])
	LDFLAGS=${LDFLAGS}" -L$with_libedit/lib"
	CPPFLAGS=${CPPFLAGS}" -I$with_libedit/include"
    fi
	
    # Check for editline/readline.h
    AC_CHECK_HEADER(editline/readline.h, broken_libedit=no, broken_libedit=yes)

    # Check for functions readline and add_history in libedit
    UNISIM_CHECK_LIB(edit, readline,
    UNISIM_CHECK_LIB(edit, add_history, broken_libedit=no,
    broken_libedit=yes),
    broken_libedit=yes)
	
    if test "$broken_libedit" == "yes"; then
		AC_MSG_NOTICE([libedit (NetBSD editline port) not found. No line edition capabilities will be available.])
    else
		LIBS="-ledit ${LIBS}"
		AC_DEFINE([HAVE_LIBEDIT], [], [Whether libedit is available])
    fi
])
