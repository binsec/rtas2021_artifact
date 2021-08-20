## UNISIM_CHECK_BOOST_MEM_FN
## Checks if the boost library is installed
## Does not take parameters
#####################################################
AC_DEFUN([UNISIM_CHECK_BOOST_MEM_FN], [
    # Check for mem_fn header
    AC_CHECK_HEADERS([boost/mem_fn.hpp],,\
	AC_MSG_ERROR([boost mem_fn header not found. Please install the boost development library. Use --with-boost=<path> to overload default includes search path.]))
])
