## UNISIM_CHECK_BOOST_BIND
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_BIND], [
    # Check for boost bind
    AC_CHECK_HEADERS([boost/bind.hpp],,\
	AC_MSG_ERROR([boost bind header not found. Please install the boost development library. Use --with-boost=<path> to overload default includes search path.]))
])
