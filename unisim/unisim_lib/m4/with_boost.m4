## UNISIM_WITH_BOOST
## Adds --with-boost on the configure command line
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_WITH_BOOST], [
    # Check if boost path has been overloaded
    AC_ARG_WITH(boost,
	AS_HELP_STRING([--with-boost=<path>], [boost library to use (will be completed with /include and /lib)]))
    if test "x$with_boost" != "x"; then
	AC_MSG_NOTICE([using boost at $with_boost])
	CPPFLAGS=${CPPFLAGS}" -I$with_boost/include"
	LDFLAGS=${LDFLAGS}" -L$with_boost/lib"
    fi
])
