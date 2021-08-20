## UNISIM_CHECK_BOOST_ARRAY
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_ARRAY], [
    # Check for some boost graph headers
    AC_CHECK_HEADERS([boost/array.hpp],,\
	AC_MSG_ERROR([boost array headers not found. Please install the boost graph development library. Use --with-boost=<path> to overload default includes search path.]))
])
