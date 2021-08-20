## UNISIM_CHECK_CACTI
## Checks if the cacti library is installed
## Does not take parameters
####################################################
AC_DEFUN([UNISIM_CHECK_CACTI], [
	# Check if cacti path has been overloaded
	AC_ARG_WITH(cacti,
	AS_HELP_STRING([--with-cacti=<path>], [Cacti 4.2 library to use (will be completed with /include and /lib)]))
	if test "x$with_cacti" != "x"; then
		AC_MSG_NOTICE([using cacti at $with_cacti])
		CPPFLAGS=${CPPFLAGS}" -I$with_cacti/include"
		LDFLAGS=${LDFLAGS}" -L$with_cacti/lib"
	fi

	# Check for cacti4_2.hh
	AC_CHECK_HEADER(cacti4_2.hh, broken_cacti=no, broken_cacti=yes)

	if test "$broken_cacti" == "yes"; then
		AC_MSG_NOTICE([CACTI 4.2 not found. Cache power estimation will be unavailable.])
	else
		# Note: we can't check libcacti4_2 functions because it's a library of C++ classes with no C functions.
		LIBS="-lcacti4_2 ${LIBS}"
		AC_DEFINE([HAVE_CACTI4_2], [], [Whether C++ wrapper class for Cacti 4.2 is available])
	fi
])
