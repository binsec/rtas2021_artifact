## UNISIM_CHECK_BOOST_INTEGER
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_INTEGER], [
    # Check for some boost integer headers
    AC_CHECK_HEADERS([boost/integer.hpp],,\
	AC_MSG_ERROR([boost integer headers not found. Please install the boost development library. Use --with-boost=<path> to overload default includes search path.]))
])
