## UNISIM_CHECK_BOOST_FUNCTION
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_FUNCTION], [
    # Check for boost function header
    AC_CHECK_HEADERS([boost/function.hpp],,\
	AC_MSG_ERROR([boost function header not found. Please install the boost development library. Use --with-boost=<path> to overload default includes search path.]))
])
