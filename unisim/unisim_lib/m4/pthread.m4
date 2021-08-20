## UNISIM_CHECK_PTHREAD
## Checks if the pthread library is installed
## Takes one parameter: the main function name
#####################################################
AC_DEFUN([UNISIM_CHECK_PTHREAD], [
    # Check for pthread.h
    AC_CHECK_HEADER(pthread.h,, AC_MSG_ERROR([pthread.h not found.]))

    # Check for functions wgetch in libncurses.a
    UNISIM_CHECK_LIB(pthread, pthread_create, broken_pthread=no, broken_pthread=yes)

    if test "$broken_pthread" == "yes"; then
		AC_MSG_NOTICE([POSIX thread library not found.])
    else
		LIBS="-lpthread ${LIBS}"
		AC_DEFINE([HAVE_PTHREAD], [], [Whether POSIX thread library is available])
    fi
])
